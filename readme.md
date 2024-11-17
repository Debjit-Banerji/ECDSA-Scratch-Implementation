# ECDSA Scratch Implementation Guide

## Implementation

For this project, we have focused on the **ECDSA** (Elliptic Curve Digital Signature Algorithm) for implementation. To implement the ECDSA from scratch, we have divided the project into three source files.

### 1. `utilities.cpp`

This file defines all the classes and functions that will be required by the other two files.

#### (a) `BigNum` Class

The `BigNum` class is designed to store and manipulate very large numbers in both decimal and hexadecimal notations, since the numbers used in ECC are large (256 bits). The class contains two strings, `str` and `decimalValue`, to store the hexadecimal and decimal representations of the large number.

##### Methods:

- **Default Constructor:** Initializes hexadecimal and decimal notations as 0.
- **Parameterized Constructor:** Initializes the values of either the hexadecimal or the decimal notation, depending on the value of the parameter `f`, and the other notation is automatically initialized using the BIGNUM module for conversion.
- **Getter for Hexadecimal Representation:** Returns the hexadecimal representation as a string.
- **Equality Check Function:** Compares two `BigNum` objects for equality.
- **Custom Addition Operation:** Performs addition of large numbers using the BIGNUM module (replaces scratch algorithm due to performance reasons).
- **Custom Multiplication Operation:** Multiplies large numbers using the BIGNUM module (replaces scratch algorithm).
- **Custom Modulus Operation:** Performs modulus using the BIGNUM module (replaces scratch algorithm).
- **Comparison Functions:** Includes greater than, less than, and equality checks.
- **Modular Inverse Function:** Uses BIGNUM module instead of scratch algorithm.
- **Subtraction, Division:** Implemented using BIGNUM module operations for better performance.
- **Square Root Functions:** Implemented using scratch algorithm based on the binary search idea.
- **Serialization and Deserialization Functions:** For serialising and de-serialising the BigNum object for communication between sender and receiver.

#### (b) `Point` Class

This class stores and manipulates Elliptic Curve Point objects. Each point has two fields, the `x` and `y` coordinates, which are objects of the `BigNum` class.

##### Methods:

- **Default Constructor:** Initializes `x` and `y` with empty `BigNum` values.
- **Parameterized Constructors:** Initializes the point with `x` and `y` coordinates, or just the `x` coordinate.
- **Getters for Coordinates:** Returns `BigNum` objects corresponding to the `x` and `y` coordinates.
- **Generate Y Coordinate from X:** Computes the `y` coordinate from the `x` value using the Elliptic Curve equation.
- **Parity and Identity Flag:** Returns whether the `x` coordinate is even or odd, and checks whether the point is the Identity Point.
- **Equality Check Function:** Compares the `x` and 'y' coordinates of the two points.
- **Serialization and Deserialization Functions:** For serialising and de-serialising the Point object for communication between sender and receiver.

#### (c) `Curve` Class

This class defines the Elliptic Curve used in ECDSA. It contains the parameters of the curve, including the values `a` and `b` in the equation:
\(y^{2} = x^{3} + a*x + b)\
It also includes the prime `p`, the generator point `G`, the order `n`, and the cofactor `h`.

##### Methods:

- Getter functions for all parameters.
- **Serialization and Deserialization Functions:** For serialising and de-serialising the BigNum object for communication between sender and receiver.

#### (d) Point Operations

- **Point Multiplication:** A custom function for point multiplication on the curve using iterative point addition utilising exponential scalar multiplication of Point objects.
- **Point Addition:** Custom function to handle point addition for two different points and point-doubling cases using the appropriate elliptic curve equations.

### 2. `ecdsaSender.cpp`

This file handles the sender's side of the ECDSA process.

#### Steps:

1. Initialize the `Curve` object using the default constructor.
2. Generate a private key within the range of 1 to \(n-1\).
3. Obtain the public key by performing point multiplication using the curve's generator point and the private key.
4. Take input for the message in hexadecimal format.
5. Hash the message using the **SHA256** algorithm.
6. Sign the message using the hashed message and the public key, following these steps:
   - Select a cryptographically secure random integer \(k\) from \([1, n-1]\).
   - Calculate the curve point \((x_1, y_1) = k \times G\).
   - Calculate \(r = x_1 \mod n\). If \(r = 0\), go back to step 1.
   - Calculate \(s = k^{-1}(z + r \cdot d_A) \mod n\). If \(s = 0\), go back to step 1.
   - The signature is the pair \((r, s)\).
7. Set up a TCP listening socket and wait for a communication request from the receiver.
8. After the connection is established, serialize and send the `Curve`, hashed message, public key, and signature \((r, s)\) arameters.

### 3. `ecdsaReceiver.cpp`

This file handles the receiver's side of the ECDSA process.

#### Steps:

1) Set up a TCP connection between the sender and receiver.
2) De-serialize the `Curve`, hashed message, public key, and signature \((r, s)\) sent by the sender.
3) Validate the public key:
   - Check if it is the identity point of the curve.
   - Check if the public key satisfies the elliptic curve equation.
   - Verify that multiplying the public key by the order `n` results in the identity point.
4) Validate the signature values \(r\) and \(s\), ensuring they lie in the range \([0, n-1]\).
5) Calculating \(e = HASH(m)\), where HASH is the
same function used in the signature generation.
6) Letting \(z\) be the \(L_n\) leftmost bits of \(e\).
7) Calculating:
\(u1 = z·s−1 mod n\) and \(u2 = r·s−1 mod n\)
8) Calculating the curve point:
\((x1, y1) = u1 · G + u2 · QA\)
If \((x1, y1) = O\), then the signature is invalid.
9) Finally, we check whether the value of v, i.e., x1 (mod n) that we got from the above calculations is equal to the value of r, which will successfully verify the signature if they are equal.

## Note:
All functions, methods and classes of each of the files are explained in detail in the code, using comments, for interested viewers.

## How to Run
To run the code after downloading the 3 code files, i.e., "utilities.cpp", "ecdsaReceiver.cpp" and "ecdsaSender.cpp", you need to follow the below given steps:

1) Open two Linux/ Ubuntu terminals (can also use two WSL terminals).
2) Navigate to the folder containing all the three files, in both the terminals.
3) Consider one of the terminals as the Sender and the other Receiver and remember this labelling of the terminals for the next steps.
4) In the Sender terminal, execute the command: `g++ -o sender ecdsaSender.cpp -lssl -lcrypto`
5) In the Receiver terminal, execute the command: `g++ -o recv ecdsaReceiver.cpp -lssl -lcrypto`
6) Now, first execute the command: `./sender` on the Sender terminal, before Step 8.
7) Enter the message to be hashed. This will generate the signature of the message you enetered, at the Sender end.
8) Now, execute the command: `./recv` at the Receiver terminal, this will verify the signature of the message you enter at the Receiver end.

### Note:
Some additional libraries might have to be installed to run the code correctly, corresponding to the libraries:
1) #include <openssl/sha.h>
2) #include <openssl/bn.h>
3) #include <openssl/rand.h>
4) #include <sys/socket.h>
5) #include <netinet/in.h>
6) #include <arpa/inet.h>
7) #include <sys/types.h>