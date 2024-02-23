

#include "LDataBase.hh"
#include "LShower.hh"
#include "LSimulationConfig.hh"
#include "LTelConfig.hh"
#include "TDirectory.h"
#include "TKey.h"
#include "TROOT.h"
#include "TTree.h"
#include "glog/logging.h"
#include <memory>



void LDataBase::Read(TFile* f)
{
    // 读取模拟配置
    //f->cd("simulation");
    simulation_config_dir = f->GetDirectory(simulation_config_dirname);
    if(simulation_config_dir)
    {
        runconfig_tree = (TTree*)simulation_config_dir->Get("runconfig");
        if(runconfig_tree)
        {
            runconfig_tree->SetBranchAddress("runconfig", &run_config);
            runconfig_tree->GetEntry(0);
            std::cout << "Read run config" << run_config->run_number<< std::endl;
        }
    }
    else 
    {
        LOG(WARNING) << "No simulation config directory in file " << f->GetName();
    }
    instrument_dir = f->GetDirectory(instrument_dirname);
    if( instrument_dir)
    {
        telconfig_tree = instrument_dir->Get<TTree>("telconfig");
        if(telconfig_tree)
        {
            telconfig_tree->SetBranchAddress("telconfig", &itel_config);
        }
        FillTelConfig();
    }
    else {
        LOG(WARNING) << "No instrument directory in file " << f->GetName();
    }

    // 读取模拟数据

    simulation_shower_dir = gDirectory->GetDirectory(simulation_shower_dirname);
    if(simulation_shower_dir)
    {
        shower_tree = simulation_shower_dir->Get<TTree>("shower");
        if(shower_tree)
        {
            shower_tree->SetBranchAddress("shower", &ishower);
        }
    }


}
void LDataBase::FillTelConfig()
{
    if( telconfig_tree)
    {
        std::cout << "Entries is "<< telconfig_tree->GetEntries() << std::endl;
        for( int i = 0; i < telconfig_tree->GetEntries(); i++)
        {
            telconfig_tree->GetEntry(i);
            int tel_id = itel_config->tel_id;
            auto tmp_config = std::make_shared<LRTelescopeConfig>();
            *tmp_config = *itel_config;
            tel_config->AddTel(tel_id, tmp_config);
        }
    }
}
void LDataBase::CopyShower(TFile* f)
{
    if(simulation_shower_dir)
    {
        f->mkdir(simulation_config_dirname);
        auto tmp_shower_dir = f->GetDirectory(simulation_shower_dirname);
        CopyDirectory(simulation_config_dir, tmp_shower_dir);
    }

}
void LDataBase::CopyRunConfig(TFile* f)
{
    if(simulation_config_dir)
    {
        f->mkdir(simulation_config_dirname);
        auto tmp_config_dir = f->GetDirectory(simulation_config_dirname);
        CopyDirectory(simulation_config_dir, tmp_config_dir);
    }
}

void LDataBase::CopyTelConfig(TFile* f)
{
    if(instrument_dir)
    {
        f->mkdir(instrument_dirname);
        auto tmp_instrument_dir = f->GetDirectory(instrument_dirname);
        CopyDirectory(instrument_dir, tmp_instrument_dir);
    }
}
void LDataBase::CopyDirectory(TDirectory* source, TDirectory* destination) {
    source->cd();
    TIter nextkey(source->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)nextkey())) {
        const char *classname = key->GetClassName();
        source->cd(); // 每次复制前都要确保当前目录是源目录
        TObject *obj = key->ReadObj();
        if (obj->InheritsFrom(TDirectory::Class())) {
            // 如果对象是一个目录，递归复制
            destination->cd();
            destination->mkdir(obj->GetName());
            TDirectory *subdir = (TDirectory*)obj;
            TDirectory *newdir = destination->GetDirectory(obj->GetName());
            CopyDirectory(subdir, newdir); // 递归复制
        } else if (obj->InheritsFrom(TTree::Class())) {
            // 如果对象是一个TTree，克隆它
            //destination->cd();
            TTree *tree = (TTree*)obj;
            auto a = tree->GetName();
            LOG(INFO)<< "Source is " << a;
            auto b = tree->GetListOfBranches();
            destination->cd();
            LOG(INFO)<< "Destination is " << destination->GetName();
            auto newtree = tree->CloneTree();
            newtree->Write();
            delete newtree; // 删除克隆的树以避免内存泄漏
        } else {
            // 如果对象不是目录也不是TTree，直接复制
            LOG(INFO) << "Copy " << key->GetName();
            destination->cd();
            obj->Write(key->GetName());
        }
        delete obj; // 删除对象以避免内存泄漏
    }
}
/*
void LDataBase::CopyDirectory(TDirectory* source, TDirectory* destination)
{
    // 将当前目录设置为源目录
    source->cd();

    // 循环遍历源目录中的所有键（对象）
    TIter nextkey(source->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)nextkey())) {
        const char *classname = key->GetClassName();
        
        //TClass *cl = gROOT->GetClass(classname);
        //if (!cl) continue;
       // source->cd(); // 每次复制前都要确保当前目录是源目录
        TObject *obj = key->ReadObj();
        //if (cl->InheritsFrom(TDirectory::Class())) {
            // 如果对象是一个目录，递归复制
          //  destination->mkdir(obj->GetName()); // 在目标文件中创建新目录
           // TDirectory *subdir = (TDirectory*)obj;
            //TDirectory *newdir = destination->GetDirectory(obj->GetName());
            //CopyDirectory(subdir, newdir); // 递归复制
        //} else {
            // 如果对象不是目录，直接复制
            destination->cd(); // 切换到目标目录
            obj->Write(key->GetName());
        }
    }
*/

void LDataBase::WriteConfig(TFile* f)
{
    // 写入模拟配置
    simulation_config_dir = f->mkdir(simulation_config_dirname);
    if(simulation_config_dir)
    {
        simulation_config_dir->cd();
        runconfig_tree = new TTree("runconfig", "runconfig", 99, simulation_config_dir);
        runconfig_tree->Branch("runconfig", &run_config);
        runconfig_tree->Fill();
        //runconfig_tree->Write();
    }
    else 
    {
        LOG(WARNING) << "Can't mkdir simulation config directory in file " << f->GetName();
    }

    instrument_dir = f->mkdir(instrument_dirname);
    if( instrument_dir)
    {
        instrument_dir->cd();
        telconfig_tree = new TTree("telconfig", "telconfig", 99, instrument_dir);
        telconfig_tree->Branch("telconfig", &itel_config);
        for (const auto& itel: tel_config->GetKeys())
        {
            itel_config = (*(tel_config))[itel].get();
            telconfig_tree->Fill();
        }
    }
    else {
        LOG(WARNING) << "Can't mkdir instrument directory in file " << f->GetName();
    }
    //telconfig_tree->Write();

}
void LDataBase::WriteShower(TFile* f)
{
    if(!simulation_shower_dir)
    {
        simulation_shower_dir = simulation_dir->mkdir(simulation_shower_dirname);
        if( simulation_shower_dir)
            shower_tree = new TTree("shower", "shower event",99, simulation_shower_dir);
        else
            LOG(WARNING) << "Can't mkdir simulation shower directory in file " << f->GetName();
        shower_tree->Branch("shower", &ishower);
    }
    ishower = shower.get();
    shower_tree->Fill();
}
LDataBase::LDataBase()
{
    shower = std::make_unique<LRShower>();
    run_config = new LRSimulationRunConfig();
    tel_config = std::make_shared<LTelescopes<std::shared_ptr<LRTelescopeConfig> >>();
    ishower = new LRShower();
    itel_config = new LRTelescopeConfig();
}
void LDataBase::Close()
{
    delete ishower;
    delete itel_config;
    delete ishower;

    tel_config->Clear();
}
LDataBase::~LDataBase()
{
    delete itel_config;
    delete run_config;
}