package com.hapeki.config;


import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.web.SecurityFilterChain;
import org.springframework.web.servlet.config.annotation.CorsRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;

@Configuration
@EnableWebSecurity
public class LoginConfig
{
    private static final String[] SECURED_URLs = {"/api/**"};
    private static final String[] UNSECURED_URLs = {"/api/user/register", "/api/playlist/all", "/register.html", "/index", "/styles.css", "/register.js"};

    @Bean
    PasswordEncoder passwordEncoder()
    {
        return new CustomPasswordEncoder();
    }

    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception
    {
        return http.csrf().disable()
                .authorizeHttpRequests()
                .requestMatchers(UNSECURED_URLs).permitAll().and()
                .authorizeHttpRequests()
                .requestMatchers(SECURED_URLs).hasAuthority("USER")
                .anyRequest()
                .authenticated().and()
                .headers().frameOptions().disable().and()
                .formLogin().loginPage("/login.html").permitAll().and()
                .httpBasic().and()
                .formLogin().defaultSuccessUrl("/home.html").and()
                .logout()
                .logoutUrl("/logout") // The URL to trigger the logout
                .logoutSuccessUrl("/") // The URL to redirect after successful logout
                .invalidateHttpSession(true) // Invalidate the HttpSession on logout
                .deleteCookies("JSESSIONID") // Delete any cookies associated with the session
                .permitAll().and().build();
    }

    @Bean
    public WebMvcConfigurer corsConfigurer() {
        return new WebMvcConfigurer() {
            @Override
            public void addCorsMappings(CorsRegistry registry) {
                registry.addMapping("/api/**") // Specify your API mapping pattern
                        .allowedOrigins("*") // Allow requests from any origin (you can restrict it to specific origins)
                        .allowedMethods("GET", "POST", "PUT", "DELETE") // Allow specific HTTP methods
                        .allowedHeaders("*"); // Allow specific headers
            }
        };
    }
}
