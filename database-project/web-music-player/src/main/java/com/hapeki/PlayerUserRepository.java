package com.hapeki;

import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;

public interface PlayerUserRepository extends JpaRepository<PlayerUser, Integer>
{
    Optional<PlayerUser> findByName(String name);
}
