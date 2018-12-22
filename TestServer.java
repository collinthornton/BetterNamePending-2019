
import java.applet.Applet;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

public class TestServer {
    public static void main(String[]args) {

        int portNumber = 4444;

        try ( 
            ServerSocket serverSocket = new ServerSocket(portNumber);
            Socket clientSocket = serverSocket.accept();
            PrintWriter out = new PrintWrite(clientSocket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        )  {
            String inputLine, outputLine;

            

            while((inputLine = in.readLine()) != null) {
                outputLine = kkp.processInput
            }
        }       
    }
}
