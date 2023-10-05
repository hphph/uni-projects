package com.hapeki.controller;

import com.hapeki.*;
import jakarta.persistence.criteria.CriteriaBuilder;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.web.multipart.MultipartFile;
import org.springframework.web.bind.annotation.*;


import java.io.IOException;
import java.security.Principal;
import java.security.Security;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

@RestController
@RequestMapping("api/playlist")
public class PlaylistController
{
    @Autowired
    private SongService songService;
    @Autowired
    private PlaylistService playlistService;
    private final SongRepository songRepository;
    private final PlaylistRepository playlistRepository;
    private final PlayerUserRepository playerUserRepository;
    private final SongDataRepository songDataRepository;
    Authentication auth;
    PlaylistController(SongService songService, SongRepository songRepository, PlaylistRepository playlistRepository, PlayerUserRepository playerUserRepository, SongDataRepository songDataRepository)
    {
        this.songService = songService;
        this.songRepository = songRepository;
        this.playlistRepository = playlistRepository;
        this.playerUserRepository = playerUserRepository;
        this.songDataRepository = songDataRepository;
    }

    static record NewPlaylistRequest(String name, boolean isPrimary) {}
    static record NewSongRequest(String name) {}
    static record NewSongIdRequest(Integer songId) {}

    @PostMapping("/add")
    public Playlist addPlaylist(@RequestBody NewPlaylistRequest request)
    {
        auth = SecurityContextHolder.getContext().getAuthentication();

        Playlist playlist = new Playlist();
        playlist.setName(request.name);
        playlist.setIsPrimary(request.isPrimary);
        playlist.setOwner(playerUserRepository.findByName(auth.getName()).get());
        playlistRepository.save(playlist);
        return playlist;
    }

    @GetMapping("/id/{id}")
    public Playlist getPlaylist(@PathVariable Integer id)
    {
        return playlistRepository.findById(id).orElseThrow();
    }

    @GetMapping("/song/{id}")
    public Song getSong(@PathVariable Integer id) { return songRepository.findById((id)).orElseThrow(); }

    @GetMapping("/name/{name}")
    public List<Playlist> getPlaylists(@PathVariable String name)
    {
        return playerUserRepository.findByName(name).orElseThrow().getPlaylists();
    }

    @PostMapping("/upload/song/{id}")
    public ResponseEntity<?> uploadSong(@RequestParam("audio")MultipartFile file, @PathVariable Integer id) throws IOException
    {
        String uploadSong = songService.uploadSong(file, id);
        return ResponseEntity.status(HttpStatus.OK)
                .body(uploadSong);
    }

    @PostMapping("/add/song/{id}")
    public Song addSong(@RequestBody NewSongRequest request, @PathVariable Integer id)
    {
        Playlist playlist = playlistRepository.findById(id).orElseThrow();
        Song song = new Song();
        song.setName(request.name);
        song.setPrimary(playlist);
        songRepository.save(song);
        return song;
    }

    @PostMapping("/add/secondary/song/{id}")
    public void addSecondarySong(@RequestBody NewSongIdRequest request, @PathVariable Integer id)
    {
        Playlist playlist = playlistRepository.findById(id).orElseThrow();
        Song song = songRepository.findById(request.songId).orElseThrow();

        List<Song> secSongs = playlist.getSecondarySongs();
        if(secSongs == null) secSongs = new LinkedList<>();
        secSongs.add(song);
        playlist.setSecondarySongs(secSongs);

        List<Playlist> secPlaylists = song.getSecondary();
        if(secPlaylists == null) secPlaylists = new LinkedList<>();
        secPlaylists.add(playlist);
        song.setSecondary(secPlaylists);
        songRepository.save(song);
        playlistRepository.save(playlist);
    }

    @GetMapping("/download/song/{id}")
    public ResponseEntity<?> downloadSong(@PathVariable Integer id)
    {
        byte[] songData = songService.downloadSong(id);
        return ResponseEntity.status(HttpStatus.OK)
                .contentType(MediaType.valueOf("audio/mpeg"))
                .body(songData);
    }

    @PostMapping("/upload/image/{id}")
    public ResponseEntity<?> uploadImage(@RequestParam("image")MultipartFile file, @PathVariable Integer id) throws IOException
    {
        String uploadImage = playlistService.uploadImage(file, id);
        return ResponseEntity.status(HttpStatus.OK)
                .body(uploadImage);
    }

    @GetMapping("/download/image/{id}")
    public ResponseEntity<?> downloadImage(@PathVariable Integer id)
    {
        byte[] songData = playlistService.downloadImage(id);
        return ResponseEntity.status(HttpStatus.OK)
                .contentType(MediaType.valueOf("image/png"))
                .body(songData);
    }

    @DeleteMapping("/delete/song/{id}")
    public void deleteSong(@PathVariable Integer id)
    {
        Song toDelete = songRepository.findById(id).orElseThrow();
        if(toDelete.getSongData() != null)
            songDataRepository.delete(toDelete.getSongData());

        songRepository.deleteById(id);
    }

    @DeleteMapping("/delete/secondary/song/{id}")
    public void deleteSecondarySong(@RequestBody NewSongIdRequest request, @PathVariable Integer id)
    {
        Song song = songRepository.findById(request.songId).orElseThrow();
        Playlist playlist = playlistRepository.findById(id).orElseThrow();

        List<Song> songs = playlist.getSecondarySongs();
        songs.remove(song);

        List<Playlist> playlists = song.getSecondary();
        playlists.remove(playlist);

        playlist.setSecondarySongs(songs);
        song.setSecondary(playlists);

        songRepository.save(song);
        playlistRepository.save(playlist);
    }

    @DeleteMapping("/delete/{id}")
    public ResponseEntity<String> deletePlaylist(@PathVariable Integer id)
    {
        if(playlistRepository.findById(id).isEmpty())
            return new ResponseEntity<>(
                    "No playlist for id " + id,
                    HttpStatus.BAD_REQUEST
            );
        Playlist toDelete = playlistRepository.findById(id).get();
        playlistRepository.delete(toDelete);
        return new ResponseEntity<>(
                "Playlist deleted",
                HttpStatus.OK
        );
    }

    @GetMapping("/all")
    public List<Playlist> downloadPlaylists()
    {
        return playlistRepository.findAll();
    }
}
