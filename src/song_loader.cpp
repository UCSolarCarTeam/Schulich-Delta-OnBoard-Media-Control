#include "song_loader.h"

SongLoader::SongLoader(std::string filepath)
{
    read_song_names(filepath);
}

void SongLoader::notify_listeners(std::string song_path)
{
    for(I_SongLoaderObserver*listener:listeners) 
    {
        listener->current_song(song_path);
    }
}

void SongLoader::next_song()
{
    current_song_index = (current_song_index + 1)%files.size();
    notify_listeners(files[current_song_index]);
}

void SongLoader::previous_song()
{
    current_song_index = (current_song_index - 1)%files.size();
    notify_listeners(files[current_song_index]);
}

std::string SongLoader::next_song_name()
{
    notify_listeners(files[(current_song_index + 1)%files.size()]);
    return files[(current_song_index + 1)%files.size()];
}

std::string SongLoader::previous_song_name()
{
    notify_listeners(files[(current_song_index - 1)%files.size()]);
    return files[(current_song_index - 1)%files.size()];
}

void SongLoader::io_event(int io_command)
{
    switch(io_command)
    {
        case 1:
            next_song();

        case 2:
            previous_song();

        case 3:
            next_song_name();

        case 4:
            previous_song_name();
    }
}

bool has_suffix(const std::string& s, const std::string& suffix)
{
    return(s.size() >= suffix.size()) && equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

bool SongLoader::read_song_names(std::string dir)
{
   DIR *dp; 
   std::string filepath;
   struct dirent *dirp;
   files = std::vector<std::string>();

   if((dp  = opendir(dir.c_str())) == NULL)
   {
       return false;
   }

   while ((dirp = readdir(dp)) != NULL)
   {
        if(has_suffix(dirp->d_name, ".mp3"))
        {
             filepath = dir + "/" + dirp->d_name;
             files.push_back(std::string(filepath));
        }
   }
   closedir(dp);
   return true;
}
