//
// Created by dougfinl on 08/01/16.
//

#ifndef VISUALISER_CONSTANTS_H
#define VISUALISER_CONSTANTS_H

#include <string>


namespace visualiser {

const std::string GEL_COLOUR_FILE_PATH = "./gel.colours";
const std::string GOBO_SEARCH_PATH = "./gobos/";

/***********
 * SHADERS *
 ***********/
const std::string SHADERS_PATH = "./shaders/";

/*****************************
 * SHADER VARIABLE LOCATIONS *
 *****************************/
const int SHADER_POSITION_ATTRIBUTE = 0;
const int SHADER_NORMAL_ATTRIBUTE = 1;
const int SHADER_TEXCOORD_ATTRIBUTE = 2;
const int SHADER_MODEL_TRANSFORMATION_ATTRIBUTE = 3;

/*****************************
 * TEXTURE BINDING LOCATIONS *
 *****************************/
const int SHADER_G_NORMAL_TEXTURE = 0;
const int SHADER_G_DIFFUSE_TEXTURE = 1;
const int SHADER_G_SPECULAR_TEXTURE = 2;
const int SHADER_G_DEPTH_TEXTURE = 3;
const int SHADER_GOBO_TEXTURE = 4;
const int SHADER_SHADOW_TEXTURE = 5;

const int SHADER_MESH_DIFFUSE_TEXTURE = 0;
const int SHADER_MESH_SPECULAR_TEXTURE = 1;

const int SHADER_POSTPROCESS_TEXTURE = 0;

/**********************
 * LIGHTING CONSTANTS *
 **********************/
const int MAX_POINT_LIGHTS = 50;
const int MAX_SPOT_LIGHTS = 50;
const float DEFAULT_AMBIENT_INTENSITY = 0.005f;

const unsigned int SHADOW_MAP_SIZE = 1024;

} // namespace visualiser


#endif //VISUALISER_CONSTANTS_H
