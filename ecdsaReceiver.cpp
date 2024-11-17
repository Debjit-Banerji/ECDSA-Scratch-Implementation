#include "./utilities.cpp"

#define BUFFER_SIZE 10000

using namespace std;

// Function to obtain the public key and ECC curve parameters from the Server, the sender of the Signature
void communicate(Curve &c, BigNum &message, BigNum &r, BigNum &s, Point &publicKey)
{
    // Initialising the socket variables for the client thread
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Creating a new TCP socket for the client thread
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Socket creation error" << endl;
        return;
    }

    // Setting the server address family and port for the client thread
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8050);

    // Setting the server IP address for the client thread
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cerr << "Invalid address/Address not supported" << endl;
        close(sock);
        return;
    }

    // Connecting to the server for the client thread
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Connection Failed" << endl;
        close(sock);
        return;
    }

    // Reading the server response for the client thread
    int bytesRead = read(sock, buffer, BUFFER_SIZE);

    // Printing the server response for the client thread
    if (bytesRead == -1) {
        perror("Failed to read data (ecdsaReceiver)");
    } else if(bytesRead == 0){
        cout << "EOF in read (ecdsaReceiver)";
    }

    // Closing the socket for the client thread
    close(sock);
    
    // Note: while sending data from the server, keep a 'tab' between the characters of each separate thing

    string temp_buffer = buffer;

    // Defining a string stream to parse the buffer contents
    stringstream ss(temp_buffer);

    // Initialising variables to store the strings after parsing portions of the buffer string stream
    string p_temp, n_temp, G_x_temp, G_y_temp, message_temp, r_temp, s_temp, pk_x_temp, pk_y_temp;
    string temp;
    ull h_temp, a_temp, b_temp;
    
    // Deserialising the curve object
    c.deserialize(ss);
    
    // Deserialising the user's hashed message
    message.deserialize(ss);

    // Deserialising the signature parameter 'r'
    r.deserialize(ss);

    // Deserialising the signature parameter 's'
    s.deserialize(ss);

    // Deserialising the Public Key
    publicKey.deserialize(ss);
}

// Function to verify the signature using the curve parameters and public key given by the Server
bool checkPublicKeyValidity(Curve c, BigNum r, BigNum s, Point publicKey, BigNum encrypted_message)
{
    // check that publicKey is not equal to identity element
    if(publicKey.getIdentity()){
        throw runtime_error("PublicKey is not valid!");
    }
    return true;
}

// Function to check if the first parameter is lesser than the second parameter
bool validNum(BigNum num, BigNum max)
{
    // Calculating the sizes of the hexadecimal strings
    int n1 = num.decimalValue.size();
    int n2 = max.decimalValue.size();

    // If the size of num's hexadecimal string is lesser, return true
    if(n1 < n2) return true;
    else if (n1 > n2) return false; // If the size of num's hexadecimal string is more, return false.
    else return num.decimalValue < max.decimalValue; // If the size of 'num's' hexadecimal string is equal to the size of 'max's' hexadecimal string, return the if the values of the 'num' string is lower than the value of the 'max' string
}

// Function to hash the message using SHA256 algorithm
BigNum hash_func(BigNum message)
{
    // Defining a buffer to store the hash message generated by the SHA256 algorithm
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // Performs the SHA256 operation on the given message to produce the encrypted message
    SHA256(reinterpret_cast<const unsigned char*>(message.get_str().c_str()), message.get_str().size(), hash);
    
    // Defining a stringstrema to iterate over the encrypted message
    stringstream ss;

    // Iterating over the encrypted message
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }

    // Initialising a BigNum variable to store the resulting hexadecimal representation of the encrypted message
    BigNum final_hash = BigNum(ss.str(), 1);

    // Returning the final result for further steps
    return final_hash;
}

int main()
{
    // Receiving the public variables from the Sender side
    Curve c;
    BigNum message;
    BigNum r;
    BigNum s;
    Point publicKeySender;
    communicate(c, message, r, s, publicKeySender);

    cout << "Publlic Variables received!" << endl;

    // Verifying that the publicKey is valid
    bool keyValid = checkPublicKeyValidity(c, r, s, publicKeySender, message);

    if(keyValid == false){
        throw runtime_error("Invalid Key!");
    }

    cout << "Public Key is Valid" << endl;

    // Verifying that r and s parameters of the signature are valid
    bool validR = validNum(r, c.get_n());

    if(validR == false) throw runtime_error("Invalid R value!");

    cout << "'r' parameter of the signature is Valid!" << endl;

    bool validS = validNum(s, c.get_n());

    if(validS == false) throw runtime_error("Invalid S value!");

    cout << "'s' parameter of the signature is Valid!" << endl;

    // Hashing the message using SHA encryption algorithm
    BigNum e = hash_func(message);

    cout << "Hashing Completed!\n";

    // Checking the validity of the 'r' and 's' values sent by the Server
    if (r.operator<=(BigNum("0", 0)) || r.operator>=(c.get_n()) || s.operator<=(BigNum("0", 0)) || s.operator>=(c.get_n())) throw runtime_error("Invalid R or S value(s)!");

    // Calculating the value of s^-1 mod n
    BigNum w = s.getInverse(c.get_n());

    // Calculating the value of e * s^-1
    BigNum u1_temp = e * w;

    // Calculating the value of (e * s^-1) mod n
    BigNum u1 = u1_temp % c.get_n();

    // Calculating the value of r * s^-1
    BigNum u2_temp = r * w;

    // Calculating the value of (r * s^-1) mod n
    BigNum u2 = u2_temp % c.get_n();

    // Calculating the value of P = (G * u1) + (publicKey * u2)
    Point P1 = pointMultiply(c.get_G(), u1, c);
    Point P2 = pointMultiply(publicKeySender, u2, c);
    Point P = pointAdd(P1, P2, c);
    
    // Calculating the value of P.x mod n, where P.x is the x coordinate of the the Point P
    BigNum v = P.get_x() % c.get_n();

    // Checking if the calculated value of ((P.x) mod n) is equal to r to verify the signature
    bool final_result = v == r;

    if(final_result == false){
        throw runtime_error("Invalid signature!");
    }

    cout << "Valid Signature!" << endl;
    cout << "Signature Verfied!" << endl;
    cout << "Process Terminating..." << endl;
}