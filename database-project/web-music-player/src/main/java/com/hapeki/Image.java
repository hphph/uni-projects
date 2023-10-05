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
@Table(name="image")
public class Image
{
    @Id
    @SequenceGenerator(
            name = "image_id_sequence",
            sequenceName = "image_id_sequence",
            allocationSize = 1
    )
    @GeneratedValue(
            strategy = GenerationType.SEQUENCE,
            generator = "image_id_sequence"
    )
    private Integer id;
    private String name;
    @JsonIgnore
    //@Column(name = "data", length = 1000000)
    private byte[] data;
    @OneToOne(mappedBy = "image")
    @JsonIgnoreProperties("image")
    private Playlist playlist;
}
