
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include<string.h>
#define PORT 8080
#define START_MENU 0
#define SIGN_UP_OPTIONS 1
#define SIGN_IN_OPTIONS 2
#define USER_OPTIONS 3
#define ADMIN_OPTIONS 4
#define SIGN_UP 5
#define SIGN_IN 6
#define SIGN_UP_AS_USER 7
#define SIGN_UP_AS_JOINT 8
#define SIGN_UP_AS_ADMIN 9
#define SIGN_IN_AS_USER 10
#define SIGN_IN_AS_JOINT 11
#define SIGN_IN_AS_ADMIN 12
#define DEPOSIT 13
#define WITHDRAW 14
#define BALANCE 15
#define PASSWORD 16
#define DETAILS 17
#define EXIT 18
#define ADD_USER 19
#define DEL_USER 20
#define MOD_USER 21
#define GET_USER_DETAILS 22
#define INVALID -1
#define BUF_SIZE 1000
int global_acc_id = 100;
struct user
{
	char type[BUF_SIZE];
	char password[BUF_SIZE];
	char u_name[BUF_SIZE];
};
struct account
{
	int balance;
	int accid;
};

int signup(int choice, char *u_name, char *password)
{
	char filename[BUF_SIZE];
	strcpy(filename, u_name);
	char extension[5] = ".txt";
	int fd;
	strncat(filename, extension, sizeof(extension));
	fd = open(filename, O_WRONLY);
	if (fd != -1)
		return -1;
	else
		close(fd);
	fd = open(filename, O_WRONLY | O_CREAT, 0644);
	if (fd == -1)
	{
		perror("signup");
		return -1;
	}
	struct user u;
	strcpy(u.u_name, u_name);
	strcpy(u.password, password);
	switch (choice)
	{
	case 7:
		strcpy(u.type, "normal");
		break;
	case 19:
		strcpy(u.type, "normal");
		break;
	case 8:
		strcpy(u.type, "joint");
		break;
	case 9:
		strcpy(u.type, "admin");
		break;
	}
	write(fd, &u, sizeof(struct user));
	struct account acc;
	acc.balance = 0;
	acc.accid = (global_acc_id += 1);
	write(fd, &acc, sizeof(struct account));
	close(fd);
	return 0;
}

int signin(int choice, char *u_name, char *password)
{
	static struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = sizeof(struct user);
	lock.l_pid = getpid();

	char filename[BUF_SIZE];
	strcpy(filename, u_name);
	char extension[5] = ".txt";
	int fd;
	strncat(filename, extension, sizeof(extension));
	fd = open(filename, O_RDONLY, 0644);
	if (fd == -1)
	{
		perror("signin");
		return -1;
	}
	struct user u;
	if (fcntl(fd, F_SETLKW, &lock) == -1)
	{
		perror("fcntl");
		return -1;
	}
	// Critical section starts........
	lseek(fd, 0, SEEK_SET);
	read(fd, &u, sizeof(struct user));
	if ((strcmp(u.password, password) != 0) || (choice == SIGN_IN_AS_USER && (strcmp(u.type, "normal") != 0)) || (choice == SIGN_IN_AS_ADMIN && (strcmp(u.type, "admin") != 0)) || (choice == SIGN_IN_AS_JOINT && (strcmp(u.type, "joint") != 0)))
		return -1;
	// Critical section ends........
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLKW, &lock);
	close(fd);
	return 0;
}

int deposit(char *u_name, int amt)
{
	static struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = sizeof(struct user);
	lock.l_whence = SEEK_SET;
	lock.l_len = sizeof(struct account);
	lock.l_pid = getpid();

	char filename[BUF_SIZE];
	strcpy(filename, u_name);
	char extension[5] = ".txt";
	int fd;
	strncat(filename, extension, sizeof(extension));
	fd = open(filename, O_RDWR, 0644);
	if (fd == -1)
	{
		perror("signin");
		return -1;
	}
	struct account acc;
	if (fcntl(fd, F_SETLKW, &lock) == -1)
	{
		perror("fcntl");
		return -1;
	}
	// Critical section starts........
	lseek(fd, sizeof(struct user), SEEK_SET);
	if (read(fd, &acc, sizeof(struct account)) == -1)
	{
		perror("read");
		return -1;
	}
	acc.balance += amt;
	lseek(fd, sizeof(struct user), SEEK_SET);
	if (write(fd, &acc, sizeof(struct account)) == -1)
	{
		perror("write");
		return -1;
	}
	// Critical section ends..........
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLKW, &lock);
	close(fd);
	return 0;
}

int withdraw(char *u_name, int amt)
{
	static struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = sizeof(struct user);
	lock.l_whence = SEEK_SET;
	lock.l_len = sizeof(struct account);
	lock.l_pid = getpid();

	char filename[BUF_SIZE];
	strcpy(filename, u_name);
	char extension[5] = ".txt";
	int fd;
	strncat(filename, extension, sizeof(extension));
	fd = open(filename, O_RDWR, 0644);
	if (fd == -1)
	{
		perror("SIGNIN");
		return -1;
	}
	struct account acc;
	if (fcntl(fd, F_SETLKW, &lock) == -1)
	{
		perror("fcntl");
		return -1;
	}
	// Critical section begins..........
	lseek(fd, sizeof(struct user), SEEK_SET);
	if (read(fd, &acc, sizeof(struct account)) == -1)
	{
		perror("read");
		return -1;
	}
	printf("balance = %d\n", acc.balance);
	acc.balance -= amt;
	if (acc.balance < 0)
		return -1;
	lseek(fd, sizeof(struct user), SEEK_SET);
	if (write(fd, &acc, sizeof(struct account)) == -1)
	{
		perror("write");
		return -1;
	}
	// Critical section ends...............
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLKW, &lock);
	close(fd);
	return 0;
}

int balance(char *u_name)
{
	static struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_start = sizeof(struct user);
	lock.l_whence = SEEK_SET;
	lock.l_len = sizeof(struct account);
	lock.l_pid = getpid();

	char filename[BUF_SIZE];
	strcpy(filename, u_name);
	char extension[5] = ".txt";
	int fd;
	strncat(filename, extension, sizeof(extension));
	fd = open(filename, O_RDONLY, 0644);
	if (fd == -1)
	{
		perror("SIGNIN");
		return -1;
	}
	struct account acc;
	if (fcntl(fd, F_SETLKW, &lock) == -1)
	{
		perror("fcntl");
		return -1;
	}
	// Critical section .......... begins
	lseek(fd, sizeof(struct user), SEEK_SET);
	if (read(fd, &acc, sizeof(struct account)) == -1)
	{
		perror("read");
	}
	// Critical section ......... ends
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLKW, &lock);
	close(fd);
	return acc.balance;
}

int change_password(char *u_name, char *pwd)
{
	static struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = sizeof(struct user);
	lock.l_pid = getpid();

	char filename[BUF_SIZE];
	strcpy(filename, u_name);
	char extension[5] = ".txt";
	int fd;
	strncat(filename, extension, sizeof(extension));
	fd = open(filename, O_RDWR, 0644);
	if (fd == -1)
	{
		perror("Password change");
		return -1;
	}
	struct user u;
	lseek(fd, 0, SEEK_SET);
	if (fcntl(fd, F_SETLKW, &lock) == -1)
	{
		perror("fcntl");
		return -1;
	}
	// Critical section begins...........
	if (read(fd, &u, sizeof(struct user)) == -1)
	{
		perror("read");
		return -1;
	}
	strcpy(u.password, pwd);
	lseek(fd, 0, SEEK_SET);
	if (write(fd, &u, sizeof(struct user)) == -1)
	{
		perror("write");
		return -1;
	}
	// Critical section ends...........
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLKW, &lock);
	close(fd);
	return 0;
}

char *get_details(char *u_name)
{
	static struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();

	char filename[BUF_SIZE];
	strcpy(filename, u_name);
	char extension[5] = ".txt";
	int fd;
	strncat(filename, extension, sizeof(extension));
	fd = open(filename, O_RDWR, 0644);
	if (fd == -1)
	{
		perror("open");
		return "user does not exist\n";
	}
	struct account acc;
	struct user u;
	if (fcntl(fd, F_SETLKW, &lock) == -1)
	{
		perror("fcntl");
		return "sorry, section is locked\n";
	}
	// sCritical section begins........
	lseek(fd, 0, SEEK_SET);
	if (read(fd, &u, sizeof(struct user)) == -1)
	{
		perror("read");
		return "unable to read file\n";
	}
	if (read(fd, &acc, sizeof(struct account)) == -1)
	{
		perror("read");
		return "unable to read file\n";
	}
	// Critical section ends..........
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLKW, &lock);
	close(fd);
	char *return_string = (char *)malloc(BUF_SIZE * sizeof(char));
	sprintf(return_string, "username : %s \npassword : %s \ntype : %s\nbalance : %d\n account id : %d\n",
			u.u_name, u.password, u.type, acc.balance, acc.accid);
	return return_string;
}
int del_user(char *u_name)
{
	static struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();

	char filename[BUF_SIZE];
	strcpy(filename, u_name);
	char extension[5] = ".txt";
	strncat(filename, extension, sizeof(extension));
	int fd = open(filename, O_RDWR, 0644);
	if (fd == -1)
	{
		perror("open");
	}
	if (fcntl(fd, F_SETLKW, &lock) == -1)
	{
		perror("fcntl");
	}

	return unlink(filename);
}

int modify_user(char *u_name, char *new_u_name, char *password)
{
	static struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();
	char filename[BUF_SIZE];
	strcpy(filename, u_name);
	char extension[5] = ".txt";
	int fd, choice;
	strncat(filename, extension, sizeof(extension));
	fd = open(filename, O_RDWR, 0644);
	if (fd == -1)
	{
		perror("mod user");
		return -1;
	}
	struct user u;
	if (fcntl(fd, F_SETLKW, &lock) == -1)
	{
		perror("fcntl");
	}
	// Critical section starts........
	lseek(fd, 0, SEEK_SET);
	if (read(fd, &u, sizeof(struct user)) == -1)
	{
		perror("read");
		return -1;
	}
	del_user(u_name);
	if (strcmp(u.type, "normal") == 0)
		choice = SIGN_UP_AS_USER;
	else
		choice = SIGN_UP_AS_JOINT;
	strcpy(u.u_name, new_u_name);
	signup(choice, new_u_name, password);
	// Critical section ends.......
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLKW, &lock);
	close(fd);
	return 0;
}
void *connection_handler(void *socket_desc)
{
	int sd = *(int *)socket_desc, choice, deposit_amt, withdraw_amt, ret, balance_amt;
	char *u_name = (char*)malloc(BUF_SIZE * sizeof(char));
	char *password = (char*)malloc(BUF_SIZE * sizeof(char));
	while (1)
	{
		char *type = (char*)malloc(BUF_SIZE * sizeof(char));
		char *new_u_name = (char*)malloc(BUF_SIZE * sizeof(char));
		char *message = (char*)malloc(BUF_SIZE * sizeof(char));
		char *option_string = (char*)malloc(BUF_SIZE * sizeof(char));
		char *amt_string = (char*)malloc(BUF_SIZE * sizeof(char));
		printf("socket = %d\n", sd);
		read(sd, option_string, sizeof(option_string));
		choice = atoi(option_string);
		if (choice == SIGN_UP_AS_USER || choice == SIGN_UP_AS_ADMIN || choice == SIGN_UP_AS_JOINT)
		{
			read(sd, u_name, sizeof(u_name));
			read(sd, password, sizeof(password));
			int ret = signup(choice, u_name, password);
			if (ret == -1)
				message = "Failed to add user please try again";
			else
				message = "User added successfully!\n";
		}
		else if (choice == SIGN_IN_AS_USER || choice == SIGN_IN_AS_ADMIN || choice == SIGN_IN_AS_JOINT)
		{
			read(sd, u_name, sizeof(u_name));
			read(sd, password, sizeof(password));
			ret = signin(choice, u_name, password);
			if (ret == -1)
				message = "Sign in failed\n";
			else
				message = "Successfully signed in!\n";
		}
		else if (choice == DEPOSIT)
		{
			read(sd, amt_string, sizeof(amt_string));
			deposit_amt = atoi(amt_string);
			ret = deposit(u_name, deposit_amt);
			if (ret == 0)
				message = "Amount deposited successfully\n";
		}
		else if (choice == WITHDRAW)
		{
			read(sd, amt_string, sizeof(amt_string));
			withdraw_amt = atoi(amt_string);
			ret = withdraw(u_name, withdraw_amt);
			if (ret == -1)
				message = "Withdraw failed";
			else
				message = "Withdraw success\n";
		}
		else if (choice == BALANCE)
		{
			balance_amt = balance(u_name);
			sprintf(message, "%d", balance_amt);
		}
		else if (choice == PASSWORD)
		{
			read(sd, password, sizeof(password));
			ret = change_password(u_name, password);
			if (ret == -1)
				message = "Password change failed\n";
			else
				message = "Password changed successfully";
		}

		else if (choice == DEL_USER)
		{
			char *u_name = (char*)malloc(BUF_SIZE * sizeof(char));
			char *password = (char*)malloc(BUF_SIZE * sizeof(char));
			read(sd, u_name, sizeof(u_name));
			ret = del_user(u_name);
			printf("unlink returned %d\n", ret);
			if (ret == -1)
				message = "Delete user failed\n";
			else
				message = "User deleted successfully\n";
		}
		else if (choice == GET_USER_DETAILS)
		{
			char *u_name = (char*)malloc(BUF_SIZE * sizeof(char));
			read(sd, u_name, sizeof(u_name));
			printf("u_name = %s\n", u_name);
			message = get_details(u_name);
		}

		else if (choice == MOD_USER)
		{
			char *u_name = (char*)malloc(BUF_SIZE * sizeof(char));
			char *password = (char*)malloc(BUF_SIZE * sizeof(char));

			read(sd, u_name, sizeof(u_name));
			read(sd, new_u_name, sizeof(new_u_name));
			read(sd, password, sizeof(password));

			ret = modify_user(u_name, new_u_name, password);
			if (ret == -1)
				message = "	Unable to modify user\n";
			else
				message = "User details updated\n";
		}

		else if (choice == DETAILS)
		{
			message = get_details(u_name);
		}

		else if (choice == ADD_USER)
		{
			char *u_name = (char*)malloc(BUF_SIZE * sizeof(char));
			char *password = (char*)malloc(BUF_SIZE * sizeof(char));
			read(sd, type, sizeof(type));
			read(sd, u_name, sizeof(u_name));
			read(sd, password, sizeof(password));
			printf("type = %s u_name = %s pwd = %s\n", type, u_name, password);
			if (!strcmp(type, "1"))
				choice = SIGN_UP_AS_USER;
			else
				choice = SIGN_UP_AS_JOINT;
			ret = signup(choice, u_name, password);
			if (ret == -1)
				message = "Failed to add account please try again later \n";
			else
				message = "Account added successfully\n";
		}
		send(sd, message, BUF_SIZE * sizeof(char), 0);
	}
	return 0;
}
