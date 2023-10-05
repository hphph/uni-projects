package com.hapeki;

import com.hapeki.controller.PlaylistController;
import jakarta.transaction.Transactional;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.EnableTransactionManagement;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.util.Optional;

@Service
public class PlaylistService
{
    @Autowired
    private PlaylistRepository playlistRepository;

    @Transactional
    public String uploadImage(MultipartFile file, Integer playlistId) throws IOException
    {
        Playlist playlist = playlistRepository.findById(playlistId).orElseThrow();
        Image img = new Image();
        img.setData(file.getBytes());
        img.setName(file.getOriginalFilename());
        playlist.setImage(img);
        playlistRepository.save(playlist);

        return "file uploaded successfully : " + file.getOriginalFilename();
    }

    public byte[] downloadImage(Integer id)
    {
        return playlistRepository.findById(id).orElseThrow().getImage().getData();
    }

}
