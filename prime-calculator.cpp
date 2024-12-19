#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

bool is_prime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

int nth_prime(int n) {
    int count = 0;
    int num = 2;
    while (true) {
        if (is_prime(num)) {
            count++;
            if (count == n) return num;
        }
        num++;
    }
}

int main() {
    int pipe_parent_to_child[2];
    int pipe_child_to_parent[2];
    pid_t pid;

    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) {
        std::cerr << "Pipe creation failed!" << std::endl;
        return 1;
    }

    pid = fork();
    if (pid == -1) {
        std::cerr << "Fork failed!" << std::endl;
        return 1;
    }

    if (pid > 0) {
        close(pipe_parent_to_child[0]);
        close(pipe_child_to_parent[1]);

        while (true) {
            int m;
            std::cout << "[Parent] Please enter the number: ";
            std::cin >> m;
            if (std::cin.fail()) {
                std::string command;
                std::cin.clear();
                std::cin >> command;
                if (command == "exit") break;
                std::cout << "Invalid input! Please enter a valid number." << std::endl;
                continue;
            }

            std::cout << "[Parent] Sending " << m << " to the child process..." << std::endl;
            write(pipe_parent_to_child[1], &m, sizeof(m));
            std::cout << "[Parent] Waiting for the response from the child process..." << std::endl;

            int result;
            read(pipe_child_to_parent[0], &result, sizeof(result));

            std::cout << "[Parent] Received calculation result of prime(" << m << ") = " << result << "..." << std::endl;
        }

        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);
        wait(NULL);
    } else {
        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);

        while (true) {
            int m;
            read(pipe_parent_to_child[0], &m, sizeof(m));

            std::cout << "[Child] Calculating " << m << "-th prime number..." << std::endl;
            int result = nth_prime(m);

            std::cout << "[Child] Sending calculation result of prime(" << m << ")..." << std::endl;
            write(pipe_child_to_parent[1], &result, sizeof(result));
        }

        close(pipe_parent_to_child[0]);
        close(pipe_child_to_parent[1]);
        exit(0);
    }

    return 0;
}

