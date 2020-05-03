//
// Created by dougfinl on 25/01/16.
//

#include "libvisualiser/ColourManager.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>


visualiser::ColourManager::ColourManager(const std::string &gelFilePath) :
    _gelFilePath(gelFilePath) {
}

visualiser::ColourManager::~ColourManager() {
}

glm::vec3 visualiser::ColourManager::lightTosRGB(const unsigned int lampColourTemp, const std::string &gelCode) const {
    glm::vec3 result(1.0f);

    if (!gelCode.empty()) {
        YAML::Node gelNode = getGelFromFile(gelCode);

        glm::vec3 xyY = glm::vec3(0.0f);
        YAML::Node xyYNode = gelNode["xyY"];
        if (xyYNode) {
            float x = xyYNode[0].as<float>();
            float y = xyYNode[1].as<float>();
            float Y = xyYNode[2].as<float>();

            xyY = glm::vec3(x, y, Y);
        } else {
            BOOST_LOG_TRIVIAL(warning) << "Could not find CIE xyY values for gel \"" << gelCode << "\"";
        }

        // Use 6774K (illuminant C as a default reference colour temperature).
        unsigned int srcColourTemp = 6774;
        YAML::Node srcNode = gelNode["src"];
        if (srcNode) {
            srcColourTemp = srcNode.as<unsigned int>();
        } else {
            BOOST_LOG_TRIVIAL(warning) << "Could not find reference colour temperature for gel \"" << gelCode << "\"";
        }

        BOOST_LOG_TRIVIAL(trace) << "Converting " << gelCode;

        glm::vec3 XYZ = xyYToXYZ(xyY);

        // Convert from source-c (gel reference white) to lamp colour temperature.
        result = chromaticAdaptation(XYZ, srcColourTemp, lampColourTemp);
    } else {
        // No gel.
        result = colourTempToXYZ(lampColourTemp);
    }

    // Convert the colour to a D65 white point (to match the sRGB colour space).
    // result = chromaticAdaptation(result, lampColourTemp, D65_WHITE_POINT);

    // Covert the XYZ tristimukus to sRGB.
    result = XYZTosRGB(result);

    return result;
}

YAML::Node visualiser::ColourManager::getGelFromFile(const std::string &gelCode) const {
    std::ifstream fin(_gelFilePath);
    if (!fin) {
        BOOST_LOG_TRIVIAL(error) << "Unable to open gel file \"" << _gelFilePath << "\"";
        return YAML::Node();
    }

    YAML::Node doc = YAML::Load(fin);
    YAML::Node gelsNode = doc["gel"];
    YAML::Node result;

    // Search for the correct gel
    for (unsigned int i = 0; i < gelsNode.size(); i++) {
        std::string code = gelsNode[i]["code"].as<std::string>();

        // If the correct node is found...
        if (code == gelCode) {
            result = gelsNode[i];
            break;
        }
    }

    return result;
}

glm::vec3 visualiser::ColourManager::colourTempToXYZ(const unsigned int kelvin) {
    // Calculate the spectral radiance for each wavelength in CIE_1931_STANDARD_OBSERVER.
    const size_t arraySize = sizeof(CIE_1931_STANDARD_OBSERVER) / sizeof(CIE_1931_STANDARD_OBSERVER[0]);
    double spectralRadiance[arraySize];

    // Fill the spectral radiance array.
    for (size_t i=0; i<arraySize; i++) {
        // Wavelength in metres.
        double lambda = CIE_1931_STANDARD_OBSERVER[i][0] * 1e-9;
        // Planck's law.
        spectralRadiance[i] = (2 * PLANCK_CONSTANT * pow(SPEED_LIGHT, 2))
            / (pow(lambda, 5) * (exp(PLANCK_CONSTANT * SPEED_LIGHT / (lambda * kelvin * BOLTZMANN_CONSTANT))-1));
    }

    // Find the maximum spectral radiance value.
    double maxSpectralRadiance = 0;
    for (size_t i=0; i<arraySize; i++) {
        maxSpectralRadiance = fmax(spectralRadiance[i], maxSpectralRadiance);
    }

    // Normalise the spectral radiance array, so its values lie in the range 0-1.
    for (size_t i=0; i<arraySize; i++) {
        spectralRadiance[i] = spectralRadiance[i] / maxSpectralRadiance;
    }

    // Calculate the final X, Y and Z values by finding the sum of multiplying each normalised spectral radiance by the
    // corresponding x-bar, y-bar, or z-bar value in the CIE_1931_STANDARD_OBSERVER.
    double X = 0;
    double Y = 0;
    double Z = 0;
    for (size_t i=0; i<arraySize; i++) {
        X += spectralRadiance[i] * CIE_1931_STANDARD_OBSERVER[i][1];
        Y += spectralRadiance[i] * CIE_1931_STANDARD_OBSERVER[i][2];
        Z += spectralRadiance[i] * CIE_1931_STANDARD_OBSERVER[i][3];
    }

    return normaliseToY(glm::vec3(X, Y, Z));
}

glm::vec3 visualiser::ColourManager::xyYToXYZ(const glm::vec3 &xyY) {
    glm::vec3 result;

    // X = x(Y / y)
    // Y = Y
    // Z = (1 - x - y)(Y / y)
    float X = xyY[0] * (xyY[2] / xyY[1]);
    float Y = xyY[2];
    float Z = (1 - xyY[0] - xyY[1]) * (xyY[2] / xyY[1]);

    result = glm::vec3(X, Y, Z);

    BOOST_LOG_TRIVIAL(trace) << "Converted xyY " << glm::to_string(xyY) << " to XYZ " << glm::to_string(result);

    return result;
}

glm::vec3 visualiser::ColourManager::XYZTosRGB(const glm::vec3 &XYZ) {
    glm::vec3 result;

    // Do the conversion (transpose the matrix, since GLM uses column-major matrices).
    result = glm::mat3(glm::transpose(glm::make_mat3(XYZ_TO_SRGB))) * XYZ;
    // Scale the RGB result from 0-100 to 0-1.
    result /= 100.0f;

    // Ensure final RGB values are in the range 0-1.
    result = glm::clamp(result, glm::vec3(0.0f), glm::vec3(1.0f));

    BOOST_LOG_TRIVIAL(trace) << "Converted XYZ " << glm::to_string(XYZ) << " to sRGB " << glm::to_string(result);

    return result;
}

glm::vec3 visualiser::ColourManager::chromaticAdaptation(const glm::vec3 &XYZ,
                                                         const unsigned int srcKelvin,
                                                         const unsigned int destKelvin) {
    const glm::vec3 srcXYZ  = colourTempToXYZ(srcKelvin);
    const glm::vec3 destXYZ = colourTempToXYZ(destKelvin);

    // Construct the chromatic adaptation transformation for Bradford CAT. (Note: inverse since GLM uses column-major
    // matrices.
    const glm::mat3 mCat = glm::inverse(glm::make_mat3(BRADFORD));

    // Calculate the cone response domains of the source and destination colour temperatures.
    const glm::vec3 srcCRD  = mCat * srcXYZ;
    const glm::vec3 destCRD = mCat * destXYZ;

    // Assemble the colour gain matrix.
    glm::mat3 gain = glm::mat3(0.0);
    gain[0][0] = destCRD[0] / srcCRD[0];
    gain[1][1] = destCRD[1] / srcCRD[1];
    gain[2][2] = destCRD[2] / srcCRD[2];

    // Calculate the final transformation matrix.
    const glm::mat3 M = glm::inverse(mCat) * gain * mCat;

    return M * XYZ;
}

glm::vec3 visualiser::ColourManager::normaliseToY(const glm::vec3 &XYZ) {
    if (XYZ.y == 0) {
        // Don't divide by 0;
        return XYZ;
    } else {
        return XYZ / XYZ.y * glm::vec3(100.0);
    }
}
