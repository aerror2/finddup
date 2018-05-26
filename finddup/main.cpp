//
//  main.cpp
//  finddup
//
//  Created by dany on 2018/5/26.
//  Copyright © 2018年 dany. All rights reserved.
//

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <openssl/md5.h>
#include <stdio.h>

using namespace boost::filesystem;
//
//int main(int argc, char *argv[]) {
//    path p(argc>1? argv[1] : ".");
//
//    if(is_directory(p)) {
//        std::cout << p << " is a directory containing:\n";
//
//        for(auto& entry : boost::make_iterator_range(directory_iterator(p), {}))
//            std::cout << entry << "\n";
//    }
//}

std::map<std::string, std::list<std::string> > simple_name_to_full_map;
std::map<std::string, std::list<std::string> > md5_to_full_map;
size_t process_file_num = 0;
void process_one_file(path p)
{
    
    std::cerr << "\rfind " << process_file_num++;
    
    std::string fn = p.filename().generic_string();
   std::transform(fn.begin(), fn.end(), fn.begin(), ::tolower);
    if(simple_name_to_full_map.find(fn)==simple_name_to_full_map.end())
    {
        simple_name_to_full_map[fn] = std::list<std::string>();
    }
    simple_name_to_full_map[fn].push_back(p.generic_string());
    
    
}

void findAll(path p)
{
    if(is_directory(p))
    {
        for(auto& entry : boost::make_iterator_range(directory_iterator(p), {}))
        {
              if(is_directory(entry) && entry != "." && entry != "..")
              {
                  findAll(entry);
              }
              else
              {
                  process_one_file (entry);
              }
        }
    }
    else
    {
        process_one_file (p);
    }
}



std::string domd5(std::string & fileName)
{
    FILE * fp = fopen(fileName.c_str(), "rb");
    if(fp==NULL)
        return std::string();
    unsigned char szBuff[4096];
    
    unsigned char rawmd5[20]={0};
    
    MD5_CTX c;
    
    size_t len;
    
    MD5_Init(&c);
    while( 0 != (len = fread(szBuff, 1, sizeof(szBuff), fp) ) )
    {
        MD5_Update(&c, szBuff, len);
    }
    MD5_Final(rawmd5,&c);
    fclose(fp);
    
    char szHash[40];
    for(int i=0;i<16;i++)
    {
        sprintf(szHash +i*2, "%02x", (unsigned char)rawmd5[i]);
    }
    
    return szHash;
}

void process_all_file_map_maybe_dup()
{
    size_t donenum = 0;
    std::cerr << "\n\n";
    
    for(auto &it : simple_name_to_full_map)
    {
        if(it.second.size() > 1)
        {
            for(auto it3 : it.second)
            {
                
                std::string hash = domd5(it3);
                std::cerr << "\rprocess " << donenum++;
                if(hash.empty())
                {
                    std::cerr << it3 << " hash is empty, read error!\n";
                }
                else
                {
                    if(md5_to_full_map.find(hash)==md5_to_full_map.end())
                    {
                        md5_to_full_map[hash] =std::list<std::string>();
                    }
                    
                    md5_to_full_map[hash].push_back(it3);
                }
            
            }
        }
    }
    
    
    std::cerr << "\n\nduplicat  file list: \n";
    
    for(auto &it2 : md5_to_full_map)
    {
         if(it2.second.size() > 1)
         {
             std::cerr << "+++++ keep " << it2.second.front() << " (md5 " << it2.first <<  ")\n";
             int k=0;
             for(auto &it4 : it2.second)
             {
                 if(k!=0)
                 {
                     std::cout << it4<< "\n";
                 }
                 k++;
             }
         }
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    path p(argc>1? argv[1] : ".");
    
    findAll(p);
    process_all_file_map_maybe_dup();
    return 0;
}
