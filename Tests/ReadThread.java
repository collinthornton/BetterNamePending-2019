import java.io.*;
import java.net.*;

/**
 * This thread is responsible for reading server's input and printing it to the
 * console. It runs in an infinite loop until the client disconnects from the
 * server.
 *
 * @author www.codejava.net
 */
public class ReadThread extends Thread {
    private BufferedReader reader;
    private Socket socket;
    private ClientMain client;
    boolean gotUsername = false;

    public ReadThread(Socket socket, ClientMain client) {
        this.socket = socket;
        this.client = client;

        try {
            InputStream input = socket.getInputStream();
            reader = new BufferedReader(new InputStreamReader(input));
        } catch (IOException e) {
            StdOut.println("Error getting input stream: " + e.getMessage());
            // e.printStackTrace();
        }
    }

    public void run() {

        while (true) {
            try {
                String response = reader.readLine();
                if (!gotUsername) {
                    // First response from server will be your username (IP address)
                    String userName = response;
                    client.setUserName(userName);
                    this.gotUsername = true;
                }
                StdOut.println("\n" + response);

                // prints the username after displaying the server's message
                if (client.getUserName() != null) {
                    StdOut.print("[" + client.getUserName() + "]: ");
                }
            } catch (IOException e) {
                StdOut.println("Error reading from server: " + e.getMessage());
                // e.printStackTrace();
                break;
            }
        }
    }
}