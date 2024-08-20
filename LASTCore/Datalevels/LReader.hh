/*
 * @Author: zhipzhang zhipzhang@mail.ustc.edu.cn
 * @Date: 2024-07-25 16:04:22
 * @LastEditors: zhipzhang zhipzhang@mail.ustc.edu.cn
 * @LastEditTime: 2024-08-12 09:38:56
 * @FilePath: /LAST/LASTCore/Datalevels/LReader.hh
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include <string>




class LReader
{
    std::string file_name;

    
    public:
        LReader(std::string file_name): file_name(file_name){}
        virtual void Open() = 0;
        virtual ~LReader() = default;
        virtual void Read() = 0;
        virtual void Close() = 0;
        virtual void Print() = 0;
        virtual void Write() = 0;
};