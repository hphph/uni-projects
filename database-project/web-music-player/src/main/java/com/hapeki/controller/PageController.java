package com.hapeki.controller;

import org.apache.coyote.Response;
import org.springframework.core.io.ClassPathResource;
import org.springframework.core.io.Resource;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class PageController
{
    @GetMapping("/playlist/{id}")
    ResponseEntity<Resource> getPlaylistPage(@PathVariable("id") String id)
    {
        Resource resource = new ClassPathResource("static/playlist.html");

        // Return the HTML page as the response
        return ResponseEntity.ok()
                .contentType(MediaType.TEXT_HTML)
                .body(resource);
    }
}
