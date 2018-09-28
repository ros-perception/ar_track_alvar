See [ROS wiki](http://wiki.ros.org/ar_track_alvar) for the users document.

# <img src="ar_track_alvar/readme_images/MarkerData_0.png" width="50"> ar_track_alvar

### Convention
We always consider the orientation shown below as the reference orientation of the mark. You can distinguish this orientation with the two empty squares represented by the red box on this example.

<img src="ar_track_alvar/readme_images/MarkerRef.png" width="100">

When you create a new tag, an XML file will be generated with it. On this file you will find the description of the corners of each tag like this one:
```
    <corner x="-2" y="-2" z="0" />
    <corner x="2" y="-2" z="0" />
    <corner x="2" y="2" z="0" />
    <corner x="-2" y="2" z="0" />
```
If you want to manually modify this file, assume that the corners are as follows.

<img src="ar_track_alvar/readme_images/corners.png" width="100">

Finally, we will consider the following 3D marker for each tag.

<img src="ar_track_alvar/readme_images/MarkerAxes.png" width="100">

### Identifiers
Each tags represents an identifier. In these nodes, the identifiers are numbers between 0 and 65535.
The identifiers used to create a tag will be specified in the name of the PNG, a generated XML file.

e.g :
For a single marker :
```
MarkerData_0.png
MarkerData_0.xml
```
For a cube :
```
MarkerData_1_2_3_4_5_6.png
MarkerData_1_2_3_4_5_6.xml
```
