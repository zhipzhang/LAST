

#ifndef _LTelescopes_HH
#define _LTelescopes_HH
#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>

// Template for telescope Array !!!!
// Is this template necessary? 
template <typename LTel>
class LTelescopes
{
    public:
        LTelescopes(){};

        LTel& operator[](int i) { return telescopes_dict[i]; };
        const LTel& operator[](int i) const { return telescopes_dict.at(i); };
        ~LTelescopes() // Becareful when the pointer is used in the map, using smart pointer is better.
        {
            Clear();
        }
        int GetTelNum() const {return telescope_id.size();};
        void AddTel(int tel_id, LTel tel) {std::cout << "Adding Element Tel id " << tel_id << std::endl;telescopes_dict[tel_id] = tel; telescope_id.push_back(tel_id);};
        void DeleteTel(int tel_id, int i){ telescopes_dict.erase(tel_id); telescope_id.erase(telescope_id.begin() + i);};
        std::vector<int> GetKeys() const {return telescope_id; };
        void Clear() 
        {
            if( telescope_id.size() > 0)
            {
                telescope_id.clear();
            }
            if(telescopes_dict.size() > 0)
            {
                auto a = telescopes_dict[1];
                telescopes_dict.clear();
            }
        }


    private:
        std::unordered_map<int, LTel> telescopes_dict;
        std::vector<int> telescope_id;


};

#endif