Client <br />
The client reads commands from the user, sends them to the server through FIFOc2s, and waits for the response from the server through FIFOs2c.<br />  The client can send several commands, including logging in and out, checking logged-in users, and getting process information. <br />

Server <br />
The server initializes the FIFO files and waits for commands from the client. It can handle multiple commands and send appropriate responses back to the client. The server maintains a list of users read from a users.txt file and processes login requests accordingly. <br />


From Client to Server  <br />
login: Prompts for a username and logs in if the username is valid. <br />
logout: Logs out the current user. <br />
get-logged-users: Retrieves and displays the currently logged-in users. <br />
get-proc-info: Prompts for a PID and fetches process information (functionality not fully implemented). <br />
quit: Quits the client. <br />

Server Responses <br />
Messages related to login status (success/failure). <br />
List of logged-in users. <br />
Process information (incomplete). <br />
Acknowledgement of received commands. <br />