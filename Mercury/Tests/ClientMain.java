import java.net.*;
import java.io.*;

/**
 * This is the chat client program. Type 'bye' to terminte the program.
 *
 * @author www.codejava.net
 */
public class ClientMain {
    private String hostname;
    private int port;
    private String userName;

    public ClientMain(String hostname, int port) {
        this.hostname = hostname;
        this.port = port;
    }

    public void execute() {
        try {
            Socket socket = new Socket(hostname, port);

            System.out.println("Connected to the chat server");

            new ReadThread(socket, this).start();
            new WriteThread(socket, this).start();

        } catch (UnknownHostException e) {
            System.out.println("Server not found: " + e.getMessage());
        } catch (IOException e) {
            System.out.println("I/O Error: " + e.getMessage());
        }

    }

    void setUserName(String userName) {
        this.userName = userName;
    }

    String getUserName() {
        return this.userName;
    }

    public static void main(String[] args) {
        if (args.length < 2)
            return;

        String hostname = args[0];
        int port = Integer.parseInt(args[1]);

        ClientMain client = new ClientMain(hostname, port);
        client.execute();
    }
}