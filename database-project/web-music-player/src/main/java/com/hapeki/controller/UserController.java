package com.hapeki.controller;

import com.hapeki.PlayerUser;
import com.hapeki.PlayerUserRepository;
import com.hapeki.Playlist;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.security.Principal;
import java.util.List;
import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.Set;

@RestController
@RequestMapping("api/user")
public class UserController
{
    private final PlayerUserRepository playerUserRepository;

    static record PlayerUserRequest(String name) {}

    UserController(PlayerUserRepository playerUserRepository)
    {
        this.playerUserRepository = playerUserRepository;
    }

    @PostMapping("/register")
    public ResponseEntity<?> loginUser(@RequestBody PlayerUserRequest request)
    {
        Optional<PlayerUser> userFound = playerUserRepository.findByName(request.name);
        PlayerUser user;
        if(userFound.isEmpty())
        {
            user = new PlayerUser();
            user.setName(request.name);
            playerUserRepository.save(user);
            return ResponseEntity.status(HttpStatus.OK)
                    .body(request.name);
        }
        else
        {
            user = userFound.get();
            return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE)
                    .body("{\"name\": \"" + request.name + "\"}");
        }


    }

    @GetMapping("/all")
    public List<PlayerUser> getUsers()
    {
        return playerUserRepository.findAll();
    }

    @GetMapping("/id/{id}")
    public PlayerUser getUserById(@PathVariable Integer id)
    {
        return playerUserRepository.findById(id).orElseThrow(NoSuchElementException::new);
    }

    @GetMapping("/name/{name}")
    public PlayerUser getUserByName(@PathVariable String name)
    {
        return playerUserRepository.findByName(name).orElseThrow(NoSuchElementException::new);
    }

    @GetMapping("/currentUser")
    public String getCurrentPlayerUser(Principal principal)
    {
        return principal.getName();
    }
}
