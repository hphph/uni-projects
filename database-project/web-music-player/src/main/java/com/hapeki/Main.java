package com.hapeki;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.transaction.annotation.EnableTransactionManagement;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@SpringBootApplication
@RestController
public class Main
{
    public static void main(String[] args)
    {
        SpringApplication.run(Main.class, args);
    }
}
