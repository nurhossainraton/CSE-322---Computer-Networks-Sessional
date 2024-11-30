package Client;

import Server.Server;

import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;

public class Client {

    static ObjectOutputStream cOut;
    ObjectOutputStream fout;
    static ObjectInputStream cin;
    ObjectInputStream fin;

    public static void main(String[] args) throws Exception {


        int f =0,dId=0;
        Socket clientsocket, filesocket;
        String username = null;
        String optiondetails = null;

        clientsocket = new Socket("localhost", 6666);
        filesocket = new Socket("localhost", 7777);

        cOut = new ObjectOutputStream(clientsocket.getOutputStream());
        cin = new ObjectInputStream(clientsocket.getInputStream());


//        filesocket = new Socket("localhost", 7777);
//        fout = new ObjectOutputStream(filesocket.getOutputStream());
//        fin = new ObjectInputStream(filesocket.getInputStream());

        while (true) {


            System.out.println("Press any number ");
            Scanner scanner = new Scanner(System.in);
            System.out.println("1: Login ");
            System.out.println("2: Online Clients");
            System.out.println("3: Request file name ");
            System.out.println("4: View files");
            System.out.println("5: Download File ");

            System.out.println("6: Upload private files");
            System.out.println("7: Upload public files");
            System.out.println("8. Logout");
            //System.out.println();

            //


            int option = scanner.nextInt();
            System.out.println(option);
            if (option == 1) {
                System.out.println("Enter your username");
                 username = scanner.next();
               // System.out.println(userinput);
                optiondetails = "Login " + username;
               //  System.out.println(optiondetails);
                cOut.writeObject(optiondetails);


            } else if (option == 2) {
                optiondetails = "Online";
                cOut.writeObject(optiondetails);
                cOut.flush();
                //System.out.println("Users Online "+(String) cin.readObject());
            } else if (option == 3) {
                System.out.println("Write your requested file name ");
                String input = scanner.next();
                optiondetails = "requestfile " + input;
                cOut.writeObject(optiondetails);
                cOut.flush();
                //System.out.println((String)cin.readObject());

            } else if (option == 4) {
                optiondetails = "viewfiles ";
                cOut.writeObject(optiondetails);
                cOut.flush();

            } else if (option == 5) {
                System.out.println("Download file id?");
                int input = scanner.nextInt();
                dId = input;
                optiondetails = "download " + input;
                cOut.writeObject(optiondetails);
                cOut.flush();
                f=1;


            }  else if (option == 6) {
                optiondetails = "upload private";
                cOut.writeObject(optiondetails);
                cOut.flush();
                sendFile("Client/"+"offline1.pdf");
                //cOut.flush();
            } else if (option == 7) {

                optiondetails = "upload public ";
                cOut.writeObject(optiondetails);
                cOut.flush();
//                File file = new File(input);
//                cOut.writeLong(file.length());
//                cOut.flush();
                sendFile("Client/"+"offline1.pdf");
            } else if (option == 8) {
                optiondetails = "logout";
                cOut.writeObject(optiondetails);
                cOut.flush();
            }
            //System.out.println("dsvffd");
//            String message = (String) cin.readObject();
//            System.out.println("mnc");
//            System.out.println("msg is "+message);
//            if(message.equalsIgnoreCase("download"))
//            {
//                receiveFile("C:\\Users\\Asus\\Downloads\\Documents");
//            }
           File file = new File("Downloads");
           file.mkdir();

           if(f!=0)
               receiveFile("Downloads/"+dId+".pdf");


        }


    }
    public static void receiveFile(String path) {
        try {
            int bytes = 0,received=0;
            FileOutputStream fileOutputStream = new FileOutputStream(path);
            long size = cin.readLong();// read file size
            int chunkSize = Server.chunkSize();

            System.out.println("chunksize is "+chunkSize);
            byte[] buffer = new byte[chunkSize];
            while (size > 0 && (bytes = cin.read(buffer, 0, (int) Math.min(buffer.length, size))) != -1) {
                fileOutputStream.write(buffer, 0, bytes);
                size -= bytes;// read upto file size
                received+=chunkSize;
                System.out.println("Files received in client "+ received+" bytes");
            }
            System.out.println("File is Received");
            fileOutputStream.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

        private static void sendFile(String path)
            throws Exception
    {
        int bytes = 0;
        // Open the File where he located in your pc
        File file = new File(path);
        FileInputStream fileInputStream
                = new FileInputStream(file);

        // Here we send the File to Server
        cOut.writeLong(file.length());
        // Here we  break file into chunks
        int cSize = Server.chunkSize();
        byte[] buffer = new byte[cSize];
        while ((bytes = fileInputStream.read(buffer))
                != -1) {
            // Send the file to Server Socket
            cOut.write(buffer, 0, bytes);
            cOut.flush();
        }
        // close the file here
        fileInputStream.close();
    }
}




//    public static void main(String[] args) throws IOException {
//       // Client client = new Client();
//       // client.run();
//        Socket clientsocket;
//        clientsocket = new Socket("localhost", 6666);
//
//    }



