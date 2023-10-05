package com.hapeki;

import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;

public interface SongRepository extends JpaRepository<Song, Integer>
{
    Optional<Song> findByName(String name);
}
