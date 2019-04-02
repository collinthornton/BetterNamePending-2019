import java.io.*;
import java.net.*;
import java.util.*;

public class LobbyServer {
    private int port;
    private Set<String> clientNames = new HashSet<>();
    private Set<ClientThread> clientThreads = new HashSet<>();

    public LobbyServer(int port) {
        this.port = port;
    }

    public void run() {
        try (ServerSocket serverSocket = new ServerSocket(port)) {

            StdOut.println("Lobby is listening on port " + port);

            while (true) {
                Socket socket = serverSocket.accept();
                StdOut.println("We got one!");
                StdOut.println(socket.getInetAddress());
                ClientThread newClient = new ClientThread(socket, this);
                clientThreads.add(newClient);
                newClient.start();
                broadcast(socket.getInetAddress().getHostAddress(), newClient);
                // try {
                // Thread.sleep(500);
                // } catch (Exception e) {
                // StdOut.println("buzz off");
                // }

                // newClient.sendMessage(socket.getInetAddress().getHostAddress());

            }

        } catch (IOException e) {
            StdOut.println("Well that didnt work");
            StdOut.println(e.getMessage());
            // e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        int port = 5555;
        if (args.length < 1) {
            StdOut.println("No Port Specified, defaulting to " + port + ".");
        } else {
            port = Integer.parseInt(args[0]);
        }

        LobbyServer server = new LobbyServer(port);
        server.run();
    }

    /**
     * Delivers a message from one client to others (broadcasting)
     */
    void broadcast(String message, ClientThread excludeClient) {
        for (ClientThread aClient : clientThreads) {
            if (aClient != excludeClient) {
                aClient.sendMessage(message);
            }
        }
    }

    void broadcast(String message) {
        for (ClientThread aClient : clientThreads) {
            aClient.sendMessage(message);
        }
    }

    /**
     * Stores clientname of the newly connected client.
     */
    void addClientName(String clientName) {
        clientNames.add(clientName);
    }

    /**
     * When a client is disconneted, removes the associated clientname and
     * ClientThread
     */
    void removeClient(String clientName, ClientThread aClient) {
        boolean removed = clientNames.remove(clientName);
        if (removed) {
            clientThreads.remove(aClient);
            StdOut.println("The client " + clientName + " quitted");
        }
    }

    Set<String> getClientNames() {
        return this.clientNames;
    }

    /**
     * Returns true if there are other clients connected (not count the currently
     * connected client)
     */
    boolean hasClients() {
        return !this.clientNames.isEmpty();
    }
}