package com.hapeki;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;
import java.util.Set;

@Entity
@Data
@AllArgsConstructor
@NoArgsConstructor
@Table(name="playlist")
public class Playlist
{
    @Id
    @SequenceGenerator(
            name = "playlist_id_sequence",
            sequenceName = "playlist_id_sequence",
            allocationSize = 1
    )
    @GeneratedValue(
            strategy = GenerationType.SEQUENCE,
            generator = "playlist_id_sequence"
    )
    private Integer id;
    private String name;
    @OneToOne(cascade = CascadeType.ALL)
    @JoinColumn(name = "image_id")
    @JsonIgnoreProperties("playlist")
    private Image image;
    private Boolean isPrimary;
    @ManyToOne()
    @JoinColumn(name = "player_user_id")
    @JsonIgnoreProperties("playlists")
    private PlayerUser owner;

    @OneToMany(mappedBy = "primary", cascade = CascadeType.ALL)
    @JsonIgnoreProperties({"secondary", "primary"})
    private List<Song> primarySongs;

    @ManyToMany(mappedBy = "secondary", cascade = CascadeType.ALL)
    @JsonIgnoreProperties({"secondary", "primary"})
    private List<Song> secondarySongs;
}
