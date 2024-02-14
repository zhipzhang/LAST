#EventIO Conversion
EventIO is a format written by Konrad and is used in nearlly all cherenkov telescope array simulation data (including [CTA](https://www.cta-observatory.org/), ASTRI amd HESS).

For better IO of this format, here I write a conversion program to convert the EventIO format to ROOT-based format or Fits format.In this code , I reimplment the eventio format by my own codes. But we also use the hsdata structure to handle the all data. Base_File.hh is what we used to face with the xrootd and local file(At this time (2023.10.22), we can not deal with the compressed file by xrootd protocol)

Thanks for the [PyEventIO](https://github.com/cta-observatory/pyeventio) and Konrad's source code. This code is adopted from their.

## Drawback
The SimTel_File need better interface.
