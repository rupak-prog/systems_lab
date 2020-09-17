//including necessary header files

#include <bits/stdc++.h>

#include <iostream>
#include <string>
#include <unistd.h>
#include <iomanip>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <cstdio>
#include "readline/readline.h"
#include "readline/history.h"

using namespace std;


// taking some global string for maintaining paths
// througout execution of program
char pwd[512];
string sprompt="ShiRu@linux:~";
string home= "/home/shivam";
string shellpath="";


// defining function prototyping
void init_shell(bool);
void getpwd();
vector<string> getArgs(string);
void shell_sort(string, bool);
time_t get_mtime(string);
string getfilename(string);
bool exist (const std::string& name);
int copy(vector<string>, bool);
int getCodeFromCmd(string);
int ls(vector<string>, bool);
int cat(vector<string>, bool);
mode_t getMode(string);
int makedir(vector<string>, bool);
int mysort(vector<string>, bool);
int substring(string,string,int);
int grep(vector<string>, int);
int cd(vector<string> );
vector<string> getFiles(string);


//creating function definitions


// when called with "true" flag,this functions displays 
// the current path in promt, otherwise prompt with "home" path
void init_shell(bool shellpathflag=false)
{
	if(shellpathflag){
		getpwd();
		// (pwd+12) is address of string 
		shellpath = sprompt + (pwd+12) + "$ ";
	}
	else shellpath = "ShiRu@linux:~$ ";
}

// this fucntion call returns the current working directory
void getpwd(){
	getcwd(pwd, 512);
}


// returns file(s) or path(s) in correct form
vector<string> getFiles(string cmd)
{
	vector<string> buf;
	vector<string> args = getArgs(cmd);
	//cout<<"args size:"<<args.size()<<endl;
	//for(int i=0; i<args.size(); i++)cout<<args[i]<<endl;
	string tmp="";
	int len=args.size(), iter=2, code=getCodeFromCmd(cmd);
	if(code==1 || code==3 || code%2==0)iter=1;
	else if(code==7)iter=3;
	for (; iter < len; ++iter)
	{
		if(args[iter][args[iter].length()-1]=='\\'){
			tmp+=args[iter]+" ";
		}
		else{
			///cout<<"pushed"<<endl;
			tmp+=args[iter];
			buf.push_back(tmp);
			tmp="";
		}
	}
	//cout<<"buf size:"<<buf.size()<<endl;
	//for(int i=0; i<buf.size(); i++)cout<<buf[i]<<endl;
	return buf;
}

// this parsing function, breaks the command into commands
vector<string> getArgs(string cmd)
{
	string tmp;
	vector<string> args;
	int i, ind, len;
	bool flag=false;
	tmp="";
	len = cmd.length();
	for(i=0; i<len; i++){
		if(cmd[i]==' ' && i<len && tmp!="")
		{
			//cout<<i<<" "<<tmp<<endl;
			args.push_back(tmp);
			tmp="";
			flag=false;
		}
		else if(cmd[i]!=' '){
			tmp = tmp + cmd[i];
			flag=false;
		}
		while(cmd[i]==' ' && cmd[i+1]==' ' && i<len){
			i++;
			flag=true;
		}
	}
	if(!flag && tmp!=""){
		//cout<<i<<" "<<tmp<<endl;
		args.push_back(tmp);
	}
	return args;
}


time_t get_mtime(string path)
{
    struct stat statbuf;
    if ( stat(path.c_str(), &statbuf) == -1 ) {
        return -1;
    }
    return statbuf.st_mtime;
}

string getfilename(string topath)
{
    string tmp;
    // this extract the filename and return it
    tmp = topath.substr(topath.find_last_of('\//') + 1);
    //cout<<"file to be copied: *"<<tmp<<"*"<<endl;
    return tmp;
}

bool exist (const std::string& name)
{
  struct stat buffer; 
  // ensures if file(s) exist on specified path  
  return (stat (name.c_str(), &buffer) == 0); 
}

int copy(vector<string> args, bool updateflag=false)
{
    string from, to, tmp, filename;
    int toind, fromind, veclen = args.size(), iter=1;

    to = args[veclen-1];
    if(args[1]=="-u")iter=2;
    

    // to_path is fixed and from_path is iterating over multiple paths
    for (; iter < veclen-1; ++iter)
    {
        from = args[iter];
        filename = getfilename(from);

        if(to[to.length()-1]=='/')to=to+filename;
        else to=to+"/"+filename;

        // checks if from_path exist or not
        if(!exist(from)){
            cout<<"cannot stat '"<<to<<"': No such file or directory\n";
            continue;
        }
        //else cout<<"file copied from: *"<<from<<"*\nto: *"<<to<<"*"<<endl;

        if(updateflag && (get_mtime(to)!=-1) && get_mtime(to)>get_mtime(from) ){
            //cout<<"to: "<<get_mtime(to)<<"\nfrom:"<<get_mtime(from)<<endl;
            cout<<from<<": option -u condition violated!\n";
            to = args[veclen-1];
			continue;
        }

        ifstream in{from};
        remove(const_cast<char*>(to.c_str()));
        ofstream out{to};
        // checks if to_path exist or not
        if (!out) {
            cerr << "cp: target '"<<to.substr(0, to.length()-filename.length())<<"' is not a directory" <<endl;
            return 1;
        }
        static constexpr size_t buffsize{1024};
        char buffer[buffsize];
        // while there is content in file, 
        // it reads and writes in file.
        while (in.read(buffer, buffsize)) {
            out.write(buffer, buffsize);
        }
        out.write(buffer, in.gcount());
        to = args[veclen-1];
    }
} 

// this sort the contents of file(s)
int mysort(vector<string> v, bool revflag=false)
{
	string tmp;
	vector<string> buf;

	int i=1, n = v.size();
	if(revflag)i=2;
	// iterating over file(s)
	for (; i < n; ++i)
	{
		if(v[i]=="")continue;
		ifstream filevar(v[i]);
		//checks if file is open or not
		if (filevar.is_open())
		{
			while ( getline (filevar,tmp) )
			{
				//cout<<tmp<<endl;
				buf.push_back(tmp);
				// line read is pushed into buffer
			}
			filevar.close();
		}
	  else{
	  	cout << "Couldn't open file *"<< v[i] <<"*"<<endl;
	  	return 1;
	  } 
	}
  	n = buf.size();
  	// buf contain the content of file(s)
  	sort(buf.begin(), buf.end());

  	if(revflag)for (i = n-1; i > 0; --i)cout<<buf[i]<<endl;
  	else for (i = 0; i < n; ++i)cout<<buf[i]<<endl;
  	return 0;
}

// this function converts string argument to mode_t type.
// it's returned value is passed to mkdir() function as a mode.
mode_t getMode(string mode)
{
	mode_t ans=0, modar[]={0400, 0200, 0100, 0040, 0020, 0010, 0004, 0002, 0001};
	int rwx=0, bitval=0, ind=8;
	for(rwx=3; rwx>0; rwx--){
		bitval = mode[rwx]-'0';
		//cout<<ind<<" "<<mode[rwx]<<" "<<bitval<<endl;
		while(bitval>0){
			if(bitval%2)ans += modar[ind];
			bitval /= 2;
			ind--;
		}
		bitval=0;
	}
	//cout<<"mode:*"<<ans<<"*\n";
	return ans;
}

// it calls mkdir() function call to create
// new directory with speicified permission
int makedir(vector<string> arg, bool modeflag=false)
{
	int i=1, len = arg.size();
	mode_t mode=0755;
	if(modeflag){
		i=3;
		mode = getMode(arg[2]);
	}
	for (; i < len; ++i){
		umask(0);
		if(mkdir ( arg[i].c_str(), mode))
				cout << "Error : Check path or folder name\n";
	}
}

// move the current shell prompt to specified path
int cd(vector<string> args)
{
	//cout<<args.size()<<" "<<args[1]<<endl;
	if(args.size()==1)chdir(home.c_str());
	if(args.size()>1 && chdir(args[1].c_str())<0){
    		cout<<"Path entered invalid!\n";
    		return 1;
    }
    else{
		getpwd();
		shellpath = sprompt + (pwd+12) + "$ ";
    } 

	return 0;
}

// displays the files and directory of requested path
int ls(vector<string> args, bool hid=true)
{
	DIR *dirptr=NULL;
	struct dirent* dirdata;
	struct stat isfolder;
	int arglen=args.size(), iter=0;

	// if part displays the contents of current directory
	if((arglen==1 && hid) || (!hid && arglen==2))
	{
		getpwd();
		dirptr = opendir(pwd);
		if(dirptr == NULL){
		cout<<"ls: cannot access '"<<pwd<<"': No such file or directory"<<endl;
		return 1;
		}
		while(dirdata = readdir(dirptr)){
			if(strcmp(dirdata->d_name, ".") && strcmp(dirdata->d_name, "..")){
				if(dirdata->d_name[0]=='.' && hid)continue;

				stat(dirdata->d_name,&isfolder);
				if(S_ISDIR(isfolder.st_mode))cout<<"\'"<<dirdata->d_name<<"\'\t";
				else cout<<dirdata->d_name<<"\t";
			}
		}
		closedir(dirptr);
		cout<<endl;
	}
	// else part displays the contents of specified directory
	else{
		iter = hid?1:2;
		for (; iter < arglen; ++iter)
		{
			dirptr = opendir(args[iter].c_str());

			if(dirptr == NULL){
				cout<<"ls: cannot access '"<<args[iter]<<"': No such file or directory"<<endl;
				return 1;
			}
			cout<<args[iter]<<":"<<endl;
			while(dirdata = readdir(dirptr)){
				// '.' and '..' are not diplayed
				if(strcmp(dirdata->d_name, ".") && strcmp(dirdata->d_name, "..")){
					if(dirdata->d_name[0]=='.' && hid)continue;

					stat(dirdata->d_name,&isfolder);
					if(S_ISDIR(isfolder.st_mode))cout<<"\'"<<dirdata->d_name<<"\'\t";
					else cout<<dirdata->d_name<<"\t";
				}
			}
			closedir(dirptr);
			if(iter<arglen-2)cout<<"\n"<<endl;
			else cout<<endl;
		}
	}
	return 0;
}


// displays the content of specified file(s)
int cat(vector<string> pat, bool show_lines=false)
{
	int lines=0, len, i;
	ifstream fp, fc;
	string text, curpath;
	len = pat.size();
	//cout<<"\n*in_cat_pat:"<<pat[1]<<"* len"<<len<<endl; 
	
	getpwd();
	curpath = pwd;
	i = show_lines?2:1;
	for (; i < len; ++i)
	{	
		fc.open(pat[i]);
		curpath = curpath+"/"+pat[i];
			//cout<<"curpath:"<<curpath<<"*"<<endl;
		fp.open(curpath);
		if(fc.is_open()){

			if(show_lines)while(getline(fc, text))cout<<setfill(' ')<<setw(6)<<++lines<<" "<<text<<endl;
			else while(getline(fc, text))cout<<text<<endl;
			curpath = pwd;
			fc.close();
		}
		else if(fp.is_open()){
			if(show_lines)while(getline(fp, text))cout<<setfill(' ')<<setw(6)<<++lines<<" "<<text<<endl;
			else while(getline(fp, text))cout<<text<<endl;
			curpath = pwd;
			fp.close();
		}
		else{
			{
				cout<<"cat: "<<pat[i]<<": No such file or directory"<<endl;
				//return 0;
				curpath = pwd;
				continue;
			}
		}
		
	}
	
	return 0;
}

// this function takes sentence and word and 
// perform several actions on sentence and words
// depending upon the flag passed 
int substring(string line,string str,int flag)
{
    int count=0,len1=line.size(),len2=str.size(),x;
    if(flag==0){
        for(int i=0; i<=len1-len2; i++){
            x=0;
            while(x<str.size() && line[i+x] == str[x])x++;
            if(x == len2)count++;
        }
        return count;
    }else{
        for(int i=0; i<=len1-len2; i++){
            x=0;
            while(x<len2){
                if(line[i+x] == str[x])x++;
                else if((str[x]>='a' && str[x]<='z') && (line[i+x]>='A' && line[i+x]<='Z') && (str[x]-32 == line[i+x]))x++; 
                else if((str[x]>='A' && str[x]<='Z') && (line[i+x]>='a' && line[i+x]<='z') && (str[x]+32 == line[i+x]))x++;
                else{
                    x++;
                    break;
                }
            }
            if(x == len2)count++;
        }
    }
    return count;
}


// searches the requested word in specified file
int grep(vector<string> args, int ind)
{
    bool mulex=false;
    int fi=2,x=1, strlen=args.size();
   
    if(ind==0 && strlen>3)mulex=true;
    
    if(ind!=0){
        fi=3;
        x=2;
        if(strlen>4)mulex=true;
    }
    for (; fi < strlen; ++fi)
    {
        string line,str="";
        int flag = 0;
        int total = 0,linecount=0, c=0;
        int len = args[x].size();

        str=args[x].substr(1, len-2);
        // cout<<"without:"<<str<<endl;

        ifstream fp;
        fp.open(args[fi]);
        if (!fp.is_open()){
            cout<<"Cannot found specified file.\n";
            return 1;
        }
        
        if(ind==2)flag = 1;

        while(getline(fp,line)){
            // below if else ladder deals with 
            // different flags/options specified with grep 
            c = substring(line,str,flag);
            //cout<<"\nc: "<<c<<endl;
            if(mulex && c && ind!=1)cout<<args[fi]<<":";
            if(c && ind==0){
                cout<<line<<endl;
            }else if(c && ind==1){
                total += c;
            }else if(c && ind==2){
                cout<<line<<endl;
            }else if(c && ind==3){
                cout<<linecount<<" "<<line<<endl;
            }
            linecount++;
        }
        if(total!=0)cout<<args[fi]<<":"<<total<<endl;
    }
    return 0;
}


// it returns integer after processing the command
// associated with that command
int getCodeFromCmd(string cmd)
{	
	int code, i, len, exist, ind;
	string st, realcmd;
	vector<string> v;
	map <string, int> cmd2code;
	cmd2code.insert({ "ls", 1 });
	cmd2code.insert({ "ls-a", 2 });
	cmd2code.insert({ "cd", 3 });
	cmd2code.insert({ "cat", 4 });
	cmd2code.insert({ "cat-n", 5 });
	cmd2code.insert({ "mkdir", 6 });
	cmd2code.insert({ "mkdir-m", 7 });
	cmd2code.insert({ "cp", 8 });
	cmd2code.insert({ "cp-u", 9 });
	cmd2code.insert({ "sort", 10 });
	cmd2code.insert({ "sort-r", 11 });
	cmd2code.insert({ "grep", 12 });
	cmd2code.insert({ "grep-c", 13 });
	cmd2code.insert({ "grep-i", 14 });
	cmd2code.insert({ "grep-n", 15 });

	v = getArgs(cmd);
	if(v.size()>1)realcmd = v[0]+v[1];
	else realcmd=v[0];
	//cout<<"realcmd*"<<realcmd<<"*\n";
	if(cmd2code.find(realcmd)!=cmd2code.end())return cmd2code[realcmd];
	else if(cmd2code.find(v[0])!=cmd2code.end())return cmd2code[v[0]];

	return 0;
}

int main(int argc, char const *argv[])
{	
	
	cout<<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n######################################*My Terminal*#################################\n\n"<<endl;
	init_shell(true);
	int cmdCode=-1;
	vector<string> arglist;
	string cmd, arg1, arg2;

	while ((cmd = readline(shellpath.c_str())) != "exit") {
        //cout << "[" << cmd << "]" << endl;
        // defined in 'readline' library, command is added as history
        if (cmd!="") add_history(cmd.c_str());
        else if(cmd=="" || cmd.length()==0){
        	//cout<<"Command not found!"<<endl;
        	continue;
        }
        
        arglist = getArgs(cmd);
		//cout<<"*"<<cmd<<"*"<<endl;
		///if(cmd=="exit")break;
		cmdCode = getCodeFromCmd(cmd);
		// after getting code as int from "getCodeFromCmd()"
		// that code is used for calling different function
		switch(cmdCode){
			case 1: ls(arglist); break;
			case 2: ls(arglist, false); break;
			case 3: cd(arglist); break;
			case 4: cat(arglist); break;
			case 5: cat(arglist, true); break;
			case 6:	makedir(arglist); break;
			case 7: makedir(arglist, true); break;
			case 8:	copy(arglist); break;
			case 9: copy(arglist, true); break;
			case 10: mysort(arglist); break;
			case 11: mysort(arglist, true); break;
			case 12: grep(arglist,0); break;
			case 13: grep(arglist,1); break;
			case 14: grep(arglist,2); break;
			case 15: grep(arglist,3); break;
			default: cout<<"Command not found!"<<endl;
		}

		//cout<<"Command is *"<<cmd<<"* code is "<<cmdCode;
		getpwd();
		//init_shell();
    }

	cout<<"Exiting...\n";
	return 0;
}