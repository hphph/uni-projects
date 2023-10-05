package com.hapeki;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;
import java.util.Set;

@Entity
@Data
@AllArgsConstructor
@NoArgsConstructor
@Builder
@Table(name="song")
public class Song
{
    @Id
    @SequenceGenerator(
            name = "song_id_sequence",
            sequenceName = "song_id_sequence",
            allocationSize = 1
    )
    @GeneratedValue(
            strategy = GenerationType.SEQUENCE,
            generator = "song_id_sequence"
    )
    private Integer id;
    private String name;
    @OneToOne(cascade = CascadeType.ALL)
    @JsonIgnoreProperties("song")
    private SongData songData;
    @ManyToOne()
    @JoinColumn(name = "primary_playlist_id")
    private Playlist primary;
    @ManyToMany()
    @JoinTable(
            name = "songs_playlists",
            joinColumns = @JoinColumn( name = "song_id", referencedColumnName = "id"),
            inverseJoinColumns = @JoinColumn(name = "playlist_id", referencedColumnName = "id")
    )
    private List<Playlist> secondary;
}
