package com.hapeki;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.databind.annotation.JsonSerialize;
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
@Table(name="player_user")
public class PlayerUser
{
    @Id
    @SequenceGenerator(
            name = "player_user_id_sequence",
            sequenceName = "player_user_id_sequence",
            allocationSize = 1
    )
    @GeneratedValue(
            strategy = GenerationType.SEQUENCE,
            generator = "player_user_id_sequence"
    )
    private Integer id;
    private String name;
    @OneToMany(mappedBy = "owner")
    @JsonIgnoreProperties("owner")
    private List<Playlist> playlists;
}
