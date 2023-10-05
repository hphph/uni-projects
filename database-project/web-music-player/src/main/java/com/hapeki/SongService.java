package com.hapeki;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.util.Optional;

@Service
public class SongService
{
    @Autowired
    private SongRepository songRepository;

    public String uploadSong(MultipartFile file, Integer songId) throws IOException
    {
        Song song = songRepository.findById(songId).orElseThrow();
        SongData songData = new SongData();
        songData.setData(file.getBytes());
        songData.setName(file.getOriginalFilename());
        song.setSongData(songData);
        songRepository.save(song);
        if(song != null)
        {
            return "file uploaded successfully : " + file.getOriginalFilename();
        }
        return null;
    }

    public byte[] downloadSong(Integer id)
    {
        return songRepository.findById(id).orElseThrow().getSongData().getData();
    }
}
