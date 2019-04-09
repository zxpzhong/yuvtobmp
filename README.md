# *python and c++ implement of transforming the yuv(*.yuv/*.nv12) format to bmp(*.bmp)

## File struction
 - `nv12.py`: Python implement. 
 - `nv12trans.sln`: The vs solution file that manage the C++ project
 - `nv12trans`:The dir contains the C++ files

## Python Usage
Usage demo: python nv12.py --input=a.nv12 --output=b.bmp

## C++ Usage
Modify the `nv12trans.cpp` in nv12trans dir. Modify the `yuv_path` var to your input nv12 file in line 66 and modify the `bmp_path` var to your output img path in line 67. Then recompile the whole VS solution and generate the exectueable file.
