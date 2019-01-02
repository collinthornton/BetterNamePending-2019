import java.io.*;
import java.net.*;
import java.util.*;

public class ClientThread extends Thread {
    private Socket socket;
    private LobbyServer server;
    private PrintWriter writer;

    public ClientThread(Socket socket, LobbyServer server) {
        this.socket = socket;
        this.server = server;

    }

    public void run() {
        try {
            InputStream input = socket.getInputStream();
            BufferedReader reader = new BufferedReader(new InputStreamReader(input));

            OutputStream output = socket.getOutputStream();
            writer = new PrintWriter(output, true);

            printClients();

            String clientName = socket.getInetAddress().getHostAddress();// reader.readLine();
            server.addClientName(clientName);

            String serverMessage = "New client connected: " + clientName;
            server.broadcast(serverMessage, this);

            String clientMessage;

            do {
                clientMessage = reader.readLine();
                serverMessage = "[" + clientName + "]: " + clientMessage;
                server.broadcast(serverMessage, this);

            } while (!clientMessage.equals("bye"));

            server.removeClient(clientName, this);
            socket.close();

            serverMessage = clientName + " has quitted.";
            server.broadcast(serverMessage, this);

        } catch (IOException e) {
            StdOut.println("Error in ClientThread: " + e.getMessage());
            // e.printStackTrace();
        }
    }

    /**
     * Sends a list of online clients to the newly connected client.
     */
    void printClients() {
        sendMessage(this.socket.getInetAddress().getHostAddress());
        if (server.hasClients()) {
            writer.println("Connected clients: " + server.getClientNames());
        } else {
            writer.println("No other clients connected");
        }
    }

    /**
     * Sends a message to the client.
     */
    void sendMessage(String message) {
        writer.println(message);
    }
}