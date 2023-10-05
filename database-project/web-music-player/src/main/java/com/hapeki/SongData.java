package com.hapeki;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonSubTypes;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import org.hibernate.annotations.Parameter;
import org.hibernate.annotations.Type;
import org.hibernate.type.descriptor.jdbc.BinaryJdbcType;

@Entity
@Data
@AllArgsConstructor
@NoArgsConstructor
@Table(name="song_data")
public class SongData
{
    @Id
    @SequenceGenerator(
            name = "songData_id_sequence",
            sequenceName = "songData_id_sequence",
            allocationSize = 1
    )
    @GeneratedValue(
            strategy = GenerationType.SEQUENCE,
            generator = "songData_id_sequence"
    )
    private Integer id;
    private String name;
    @JsonIgnore
    //@Column(name = "data", length = 1000000)

    private byte[] data;
    @OneToOne(mappedBy = "songData")
    @JsonIgnoreProperties("songData")
    private Song song;
}
