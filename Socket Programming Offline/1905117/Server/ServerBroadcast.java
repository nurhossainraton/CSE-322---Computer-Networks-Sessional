package Server;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.util.Scanner;

public class ServerBroadcast extends Thread{
    @Override
    public void run(){
        try{
            while(true){
                Scanner scanner = new Scanner(System.in);
                String message = scanner.nextLine();
                for(int i = 0; i<Server.clients.size(); i++){
                    if(!Server.clients.get(i).isClosed()){
                        ObjectOutputStream dataOutputStream = new ObjectOutputStream(Server.clients.get(i).getOutputStream());
                        dataOutputStream.writeObject(message);
                        dataOutputStream.flush();
                    }
                }
            }
        }catch (IOException e){
            e.printStackTrace();
        }

    }
}
