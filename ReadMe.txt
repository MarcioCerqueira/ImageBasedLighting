This is a C++/GLSL aplication which lits polygonal models and medical volumes based on the SH coefficients computed from a light probe.

The light probe can be an angular LDR (stored in common formats such as .png, .jpg) or HDR image (stored in .hdr).

Volumes can be found at the other branch VolumeRendering.

Also, if live LDR light probe capturing is desirable, one must use a configuration file like this:

Cube Map (LDR):
0

Light Probe Map:
cam
cam

which enables the webcam to capture the light probe in real-time. (Obviously, the SH Coefficients are computed in real-time. This process is done by using the approach proposed by "An Efficient Representation for Irradiance Environment Maps");