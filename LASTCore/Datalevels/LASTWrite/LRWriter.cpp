#include "LRWriter.hh"


LRWriter::LRWriter(const char mode)
{
    if(mode == 'w')
    {
        InitRootFile();
    }
    else if (mode == 'r') 
    {
        ReadRootFile();
    }
}