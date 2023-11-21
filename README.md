### Assumptions
- User is required to enter options
- Full path is no longer than 200 chars
- Name of a path cannot have spaces
- Files and Directories do not have the same name
- Error if file to write into does not exist
- Size of the paths file is no bigger than 2000 characters
- The client must enter the path relative to the ss and it must begin with '/'
- If a client is writing to a file, then the other client can't access the file (Resource Busy error)
- You cannot get permissions of a directory
- The code will NOT work with string when you are required to enter an integer (like for options)
- Assuming error codes requires us to display only one of the possible errors.
- Not printing error codes on the NM side (For bookkeeping) for read, write and get permissions becuase for these functionalities, there is direct communication only between the SS and client.
- Directories shall not be named with numbers, they are reserved for redundant storage servers
- Atleast one storage shall be alive before client can make any connection
- Paths file must end with a new line
- Define the naming server IP in the headers

### Project overview??
- To run: ./nm ./ss ./client and then ss can come at anytime
- Same ports cannot be reused by multiple storage servers
- 5566 is always reserved for the naming server and cannot be used for any other purpose
- The storage server must always have a file that stores all the accessible paths in the same directory as the ss code, whose name needs to be input at prompt
- If the parent directory of a file/folder is not accessible, the file/folder itself will not be accessible
