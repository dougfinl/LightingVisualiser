# Gel Colours

The gel colour specification file must be named `gel.colours`, and must follow the [YAML](https://yaml.org/) format.


## Specification

### Gel

Section name: `gel`

#### Required
- `code` - unique ID (usually the manufacturer's code)
- `name` - textual name for reference purposes
- `desc` - gel description
- `xyY` - [CIE 1931 xyY](https://en.wikipedia.org/wiki/CIE_1931_color_space) spectral data for the colour, formatted as `[x, y, Y]`
- `src` - colour temperature (Kelvin) of the light source when the spectral data was captured


## Example (gel.colours)

These commonly-used gel colours are made by [Lee Filters](https://www.leefilters.com/).

```yaml
gel:
  - code: "L058"
    name: "Lavender"
    desc: "An excellent backlight that creates a new dimension."
    xyY:  [0.212, 0.099, 8.9]
    src:  6774
  - code: "L071"
    name: "Tokyo Blue"
    desc: "A deep blue, used for midnight scenes and cyclorama lighting."
    xyY:  [0.151, 0.03, 1]
    src:  6774
  - code: "L103"
    name: "Straw"
    desc: "Pale sunlight through a window and a good warm winter effect."
    xyY:  [0.336, 0.359, 81.6]
    src:  6774
```
