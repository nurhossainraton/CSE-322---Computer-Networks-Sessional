package Server;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.*;

public class Server {
   public static List<Socket> clients ;
   public static List<String> user ;
   public static List <Socket> file;
   public static ObjectInputStream cin;
   public static ObjectOutputStream cout;

   int MAX_BUFFER_SIZE = 10000000;
   public static int MIN_CHUNK_SIZE = 1024;
   public static int MAX_CHUNK_SIZE = 8*1024;
   static int fileId=0;
    public static int chunkSize(){
        Random random = new Random();
        int temp = random.nextInt(MAX_CHUNK_SIZE-MIN_CHUNK_SIZE);
        return temp + MIN_CHUNK_SIZE;
    }

    public static void main(String[] args) throws Exception {
        ServerSocket cSocket= new ServerSocket(6666);
        ServerSocket fSocket = new ServerSocket(7777);

        HashMap<Socket,UserInfo> userHashMap = new HashMap<Socket, UserInfo>();
        HashMap <UserInfo,String>  reqHash = new HashMap<UserInfo, String>() ;
        String str = "";

        // while loop, so that server can connect to another client
        // after serving the current one
        while(true) {

            clients = new ArrayList<>();
            user = new ArrayList<>();
            Socket clientsocket = cSocket.accept();
            Socket filesocket = fSocket.accept();
             cin  =   new ObjectInputStream(clientsocket.getInputStream());
             cout = new ObjectOutputStream(clientsocket.getOutputStream());


            System.out.println("Connection established");
            System.out.println("Local port: " + clientsocket.getLocalPort());
            clients.add(clientsocket);
            //


            // output buffer and input buffer




//            ObjectOutputStream fout = new ObjectOutputStream(filesocket.getOutputStream());
//            ObjectInputStream fin = new ObjectInputStream(filesocket.getInputStream());
          //  file.add(filesocket);


            ServerBroadcast serverBroadcast = new ServerBroadcast();
            serverBroadcast.start();


            while(true) {

                if(clientsocket.isClosed())
                {
                    break;
                }

                String s= (String) cin.readObject();
                System.out.println("command "+s);
                String []ara = s.split("\\ ");
                //System.out.println("Command"+ara[0] + " " + ara [1]);


                if (ara[0].equalsIgnoreCase("Login")) {

                    if ( user.contains(ara[1])) {
                        //System.out.println("User Already logged in");
                        cout.writeObject("User Already logged in");
                        cout.flush();
                        clientsocket.close();

                    } else {
                        UserInfo userInfo = new UserInfo(ara[1]);
                        userHashMap.put(clientsocket, userInfo);

                        user.add(ara[1]);
                       // System.out.println("Username is " + ara[1]);
                        //System.out.println("User from index 1 is " + user.get(0));
                        File privatedirectory = new File("Storage/"+ara[1]+"/private");
                        privatedirectory.mkdirs();
                        File pubdirectory = new File("Storage/"+ara[1]+"/public");
                        pubdirectory.mkdirs();

                    }
                }
                else if (! ara[0].equalsIgnoreCase("Login")) {
                    //System.out.println("login noy");
                    if (!userHashMap.containsKey(clientsocket)) {
                        System.out.println(1);
                        cout.writeObject("You are not logged in");
                        cout.flush();

                    } else if (ara[0].equalsIgnoreCase("Online")) {

                        System.out.println("Online users : " );
                       for(String u :user)
                       {
                           System.out.println(u);
                       }
                        //System.out.println(user);

                    } else if (ara[0].equalsIgnoreCase("requestfile")) {
                       // System.out.println(3);
                        String split[] = s.split(" ", 2);
                        String reqUser = userHashMap.get(clientsocket).getUsername();
                        System.out.println( reqUser+" requested"+" file "+ split[1]);
                       // System.out.println("clients size os "+ clients.size());
                        for (int i = 0; i < clients.size(); i++) {
                            if (!clients.get(i).isClosed()) {
                               // System.out.println("req e dhukse");
                                ObjectOutputStream objectOutputStream = new ObjectOutputStream(clients.get(i).getOutputStream());
                                objectOutputStream.writeObject(reqUser + " requested for file " + split[1]);
                                objectOutputStream.flush();
                                //System.out.println("req sesh");
                            }
                        }
                        reqHash.put(userHashMap.get(clientsocket),split[1]);
                        System.out.println(reqHash);
                    }

                    else if (ara[0].equalsIgnoreCase("upload"))
                    {
                        fileId++;
                        if(ara[1].equalsIgnoreCase("private"))
                        {
                            //FileInputStream fileInputStream = new FileInputStream(");
                            receiveFile("Storage/"+user.get(user.size()-1)+"/"+"private/"+Integer.toString(fileId)+".pdf");
                        }
                        else
                        {
                            receiveFile("Storage/"+user.get(user.size()-1)+"/"+"private/"+Integer.toString(fileId)+".pdf");
                        }

                    }
                   else if(ara[0].equalsIgnoreCase("download"))
                    {
                        sendfile(MAX_CHUNK_SIZE,"Storage/"+user.get(user.size()-1)+"/"+"private/"+ Integer.toString(fileId) +".pdf" );
                    }
                    else if(ara[0].equalsIgnoreCase("viewfiles"))
                    {
                        File files = new File("Storage");
                        String allfiles = "";
                        String []cd = files.list();
                        for(int i=0;i<cd.length;i++)
                        {

                            files = new File("Storage/"+cd[i]+"/"+"private");
                            String [] privFiles = files.list();
                                for(int j=0;j<privFiles.length;j++)
                                {
                                    allfiles+=cd[i]+"_"+"private_"+privFiles[j]+"\n";
                                }

                            files = new File("Storage/"+cd[i]+"/"+"public");
                            String [] pubFiles = files.list();
                            for(int j=0;j<pubFiles.length;j++)
                            {
                                allfiles+=cd[i]+"_"+"public_"+pubFiles[j]+"\n";
                            }
                        }
                        System.out.println(allfiles);
                        cout.writeObject(allfiles);
                        cout.flush();
                    }
                   else if(ara[0].equalsIgnoreCase("logout"))
                    {
                        //System.out.println("last user is "+user.get(user.size()-1));
                        user.remove(user.get(user.size()-1));
                        //System.out.println(user);
                    }


                }

               // System.out.println(user);

            }
        }

    }

    private static void sendfile(int maxChunkSize,String path) {
        try{
            int bytes = 0;
            File file = new File(path);
          //  System.out.println(path);
            FileInputStream fileInputStream = new FileInputStream(file);

            // send file size
            cout.writeLong(file.length());
            cout.flush();
            // break file into chunks
            byte[] buffer = new byte[maxChunkSize];
            while ((bytes=fileInputStream.read(buffer))!=-1){
                cout.write(buffer,0,bytes);
                cout.flush();
            }
            fileInputStream.close();
            System.out.println(path+" Download Procedure Completed");
        }catch (Exception e){
            System.out.println("Error in file sending");
        }
    }

    private static void receiveFile(String fileName)
            throws Exception
    {
        int bytes = 0,sizedelivered=0,ack=0;
        FileOutputStream fileOutputStream
                = new FileOutputStream(fileName);
       // File file = new File(fileName);
        long size
                = cin.readLong();// read file size
        //System.out.println("file size is "+size);
        int chunkSize = chunkSize();

        System.out.println("chunksize is "+chunkSize);
        byte[] buffer = new byte[chunkSize];
        while (size > 0
                && (bytes = cin.read(
                buffer, 0,
                (int)Math.min(buffer.length, size)))
                != -1) {
            // Here we write the file using write method

                fileOutputStream.write(buffer, 0, bytes);
                size -= bytes; // read upto file size
                sizedelivered+=chunkSize;
                System.out.println("FIles received in server "+ sizedelivered+" bytes");
               // ack++;

        }
        // Here we received file

            System.out.println("File is Received");
            fileOutputStream.close();


    }
}

