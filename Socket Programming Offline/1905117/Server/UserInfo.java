package Server;

import java.io.File;


import java.io.Serializable;

public class UserInfo {
    String username;

    public UserInfo(String username) {
        this.username = username;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;

    }
}
