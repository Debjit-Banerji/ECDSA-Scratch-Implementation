#include "utilities.cpp"
# define BUFFER_SIZE = 10000

using ull = unsigned long long;
using namespace std;

// This wikipedia page was followed to implement this algorithm
// https://en.wikipedia.org/wiki/Elliptic_Curve_Digital_Signature_Algorithm

// parameters -> (CURVE, G, n)
// here CURVE is the curve used
// G is an element of curve
// n is the subgroup order of G

// The curve used is secp256k1 (y^2 = x^3 + 7)
// (a, b, p, G, n, h) -> domain parameters
// p = FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFC2F
// G (base point) = 02 79BE667E F9DCBBAC 55A06295 CE870B07 029BFCDB 2DCE28D9 59F2815B 16F81798
// The representation of G is in compressed form. Here 02 at the start means y is even. 03 at the start would mean y is odd
// n = FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE BAAEDCE6 AF48A03B BFD25E8C D0364141
// h = 01

// Function to take input for the message from the user
void takeInput(string &input)
{
    cout << "Enter the message (in hexadecimal form) " << endl;
    string original_input = "";
    input = "";

    // Taking input in hexadecimal form from the user
    getline(cin, original_input);

    // Checking if the message entered by the user is a valid hexadecimal message or not
    copy_if(original_input.begin(), original_input.end(), back_inserter(input), [](char c) -> bool
                 { return (('0' <= c && c <= '9') || ('a' <= c <= 'f')); });
    for (auto c : input)
    {
        if (!(('0' <= c && c <= '9') || ('a' <= c && c <= 'f')))
        { // If the user enters an invalid hexdecimal message, the user will be asked to re-enter the message
            cout << "Error! Incorrect Input. Please enter again." << endl;
            takeInput(input);
            return;
        }
    }
    return;
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

// Function to generate a random number within a given range
BigNum generateCPRN(BigNum n)
{
    BIGNUM* max_num = BN_new();
    BN_hex2bn(&max_num, n.get_str().c_str());

    BIGNUM* random_num = BN_new();
    int byte_count = BN_num_bytes(max_num);

    unsigned char* num = (unsigned char*)malloc(byte_count);

    // Generating the random number and checking for errors if any.
    if(RAND_bytes(num, byte_count) != 1) {
        perror("Error in generating random value in generate CPRN");
    }
    BN_bin2bn(num, byte_count, random_num);

    while(BN_cmp(max_num, random_num) < 0){
        if(RAND_bytes(num, byte_count) != 1) {
            perror("Error in generating random value in generate CPRN");
        }
        BN_bin2bn(num, byte_count, random_num);

    }

    // Converting the generated random number into the custom BigNum type
    char* random_num_c_str = BN_bn2hex(random_num);
    string random_num_str = random_num_c_str;
    BigNum b{random_num_str, 1};

    // Returning the random number for further steps
    return b;
}

// Function to generate the signature of the message
void generateSignature(Curve &c, BigNum &r, BigNum &s, BigNum &hashedMessage, BigNum &privateKey)
{
    // Runs till the time r > 0 and s > 0
    while(true){
        // Generating a random number 
        BigNum ephemeralKey = generateCPRN(c.get_n());

        // Multiplying the Generator point of the ECC curve with random number
        Point curvePoint = pointMultiply(c.get_G(), ephemeralKey, c);

        // Calculating r = curvePoint.x mod n
        r = curvePoint.get_x().operator%(c.get_n());
        if(r.operator==(BigNum("0", 0))) continue;

        // Calculating ephemeralKey_inverse
        BigNum ephemeralKeyInverse = ephemeralKey.getInverse(c.get_n());

        // Calculating s = ephemeralKeyInverse * (hashedMessage + r*privateKey) mod n
        s = ephemeralKeyInverse * ((hashedMessage + (r * (privateKey) % c.get_n())) % c.get_n()) % c.get_n();
        if(s.operator==(BigNum("0", 0))) continue;
        else break;
    }    
}

// Fucntion to send the variables from the Sender to the Receiver
void communicate(Curve c, BigNum m, BigNum r, BigNum s, Point publicKey)
{
    // Using tcp to communicate m, r, s strings with ecdsaReceiver
    int server_listening_fd;
    struct sockaddr_in server_addr, client_addr;
    int server_addrlen = sizeof(server_addr);

    // Setting the socket the Sender side
    if ((server_listening_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Listening Socket Failed");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Setting the IP address and port for the sender
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8050);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the address and port to the socket for communication
    if(bind(server_listening_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in bind syscall");
    }

    cout << "Waiting for Receiver connection!" << endl;

    // Listening for clients with upto 5 active listening but unaccepted connections
    if(listen(server_listening_fd, 5) < 0) {
        perror("Error in listen syscall\n");
    }

    int client_sock = 0;
    int client_addr_length = sizeof(client_addr);
    // accepting and creating a new socket for the client connection
    if((client_sock = accept(server_listening_fd, (struct sockaddr*)&client_addr, (socklen_t *)&client_addr_length)) < 0) {
        perror("Error in accept syscall\n");
    }

    // Defining a buffer to store the data to be sent to the receiver
    string buffer;
    stringstream buf{buffer};
    buf << c.serialize() << '|' << m.serialize() << '|' << r.serialize() << '|' << s.serialize() << '|' << publicKey.serialize();
    
    // Sending the data to the receiver
    if(send(client_sock, buf.str().c_str(), buf.str().size(), 0) == -1){
        perror("Error in sending information\n");
    }

    // Closing the connection
    close(client_sock);
}

int main()
{
    // The curve with its parameters to be considered
    Curve secp256k1;

    cout << "Curve Initialised!" << endl;

    // first create a private and public key
    BigNum privateKey = generateCPRN(secp256k1.get_n());

    cout << "PrivateKey generated!" << endl;

    Point publicKey = pointMultiply(secp256k1.get_G(), privateKey, secp256k1);

    cout << "Public Key calculated!" << endl;

    // Taking input message from the user
    string inputStrMessage;
    takeInput(inputStrMessage);

    cout << "User's message received!." << endl;

    BigNum inputMessage(inputStrMessage, 1);

    // Hashing the message of the user
    BigNum hashedMessage = hash_func(inputMessage); // Since we are using sha256 we expect to have 64 hexadecimal characters as the result

    cout << "User's Message hashed!" << endl;

    // Generating the signature of the message
    BigNum r;
    BigNum s;
    generateSignature(secp256k1, r, s, hashedMessage, privateKey);

    cout << "Signature Generated!" << endl;

    // Communicating the variables to the receiver
    communicate(secp256k1, hashedMessage, r, s, publicKey);

    cout << "Communication with the Receiver Successful!" << endl;
    cout << "Closing the Connection" << endl;
    
    return 0;
}