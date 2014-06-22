C++ implementation of: _Line Generalisation by Repeated Elimination of the Smallest Area_ ("Visvalingam's algorithm"), http://www2.dcs.hull.ac.uk/CISRG/publications/DPs/DP10/DP10.html

## Example usage
    make
    bin/simplify --file data/ne_10m_admin_0_countries.shp

## Sample data
Source data used: Natural Earth Data: http://www.naturalearthdata.com/downloads/10m-cultural-vectors/

### USA
number of points: 9119
![USA 10m_1](https://github.com/shortsleeves/visvalingam_simplify/raw/master/images/ne_10m_usa.png)

### Simplified USA
number of points: 1518
![USA 10m_2](https://github.com/shortsleeves/visvalingam_simplify/raw/master/images/ne_10m_usa_simplified.png)

### South of France
![SouthFrance_1](https://github.com/shortsleeves/visvalingam_simplify/raw/master/images/ne_10m_south_france.png)

### Simplified South of France
Same input data, same simplification thresholds as USA above.
![SouthFrance_2](https://github.com/shortsleeves/visvalingam_simplify/raw/master/images/ne_10m_south_france_simplified.png)

### Overlaid South of France
![SouthFrance_3](https://github.com/shortsleeves/visvalingam_simplify/raw/master/images/ne_10m_south_france_overlaid.png)

## Dependencies:
* C++ compiler that supports -std=c++11 (for unordered_map)
* gdal OGR, for file format support

## License
Simplified BSD License, see LICENSE
