/**
 * @file LRWriter.hh
 * @author zhipz (zhipzhang@mail.ustc.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */


 #ifndef _LRWRITER_HH_
 #define _LRWRITER_HH_

 class LRWriter
 {
    public:
        LRWriter();
        LRWriter(const char mode = 'w');
        ~LRWriter();
    protected:
        virtual void InitRootFile() {};  // Create the TFile and TTree
        virtual void ReadRootFile() {};  // Read the TFile and Init the TTree
        virtual bool HandleEvent() = 0;  // Fill the ROOT File (TTree)
        virtual void StoreTTree() {};   // Store the TTree and close the TFile
        virtual bool ReadEvent()  = 0;    // Read the entry of TTree

 };







 #endif