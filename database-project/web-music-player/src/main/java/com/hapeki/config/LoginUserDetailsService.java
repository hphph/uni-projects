package com.hapeki.config;

import com.hapeki.PlayerUserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.stereotype.Component;

@Component
public class LoginUserDetailsService implements UserDetailsService
{
    @Autowired
    private PlayerUserRepository userRepository;

    @Override
    public UserDetails loadUserByUsername(String username) throws UsernameNotFoundException
    {
        return userRepository.findByName(username)
                .map(LoginUserDetails::new)
                .orElseThrow(() -> new UsernameNotFoundException("No user found"));
    }


}
