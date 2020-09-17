#include<bits/stdc++.h>
#include<stdlib.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<vector>
#include<cstdlib>
#include<sys/wait.h> 
#include<fcntl.h>
using namespace std;

int findNewValue(vector<pair<int,int>> snake,vector<pair<int,int>> ladder,int input,int curr,int endpoint){
    int currpos = input+curr, flag1=0, flag2=0;
    
    if(currpos == endpoint){
        return input;
    }else if(currpos > endpoint){
        return -1;
    }
    
    //find current position in the snake list
    while(1){
        for(int i=0; i<snake.size(); i++){
            if(currpos<snake[i].first){
                break;
            }else{
                if(snake[i].first == currpos){
                    //snake will bit and player position will go down
                    cout<<"snake found\n";
                    currpos = snake[i].second;
                    i=0;
                    flag1 = 1;
                }
            }
        }
        for(int j=0; j<ladder.size();j++){
            if(currpos<ladder[j].first){
                break;
            }else{
                if(ladder[j].first == currpos){
                    //ladder will take player to higher value
                    cout<<"ladder found\n";
                    currpos = ladder[j].second;
                    flag2 = 1;
                }
            }
        }
        if(flag1==0 && flag2==0){
            break;
        }
        flag1 = 0;
        flag2 = 0; 
    }
return currpos;
}

bool file_is_well(vector<pair<int,int>> snake,vector<pair<int,int>> ladder,int max_val){
    //checking that mouth of snake is higher than tail of the snake
    for(int i=0; i<snake.size(); i++){
        if(snake[i].first < snake[i].second){
            cout<<"Mouth of snake should be higher than tail of the snake\n";
            return false;
        }else if(snake[i].first == snake[i].second){
            cout<<"Mouth and tail of snake should not be at same block\n";
            return false;
        }
        if(snake[i].first > max_val){
            cout<<"Mouth of the snake should not exceed gameboard size\n";
            return false;
        }
        if(snake[i].second <= 0){
            cout<<"Tail of the snake should be above 0(starting position)\n";
            return false;
        }
    }

    //checking that starting value of ladder is small than ending value of ladder
    for(int j=0; j<ladder.size(); j++){
        if(ladder[j].first > ladder[j].second){
            cout<<"Starting value of ladder is small than ending value of ladder\n";
            return false;
        }else if(ladder[j].first == ladder[j].second){
            cout<<"Starting and Ending position of ladder should not be same\n";
            return false;
        }
        if(ladder[j].second > max_val){
            cout<<"Ending location of ladder should not exceed the gameboard size\n";
            return false;
        }
        if(ladder[j].first <= 0){
            cout<<"Starting position of ladder should be above 0\n";
            return false;
        }
    }
return true;
}

int main(int argc,char *argv[]){
    int n,total,endPoint;

    //task1: Checking number of players and number of arguments
    if(argc < 4){
        cout<<"Too few arguments\n";
        return 1;
    }else if(argc > 4){
        cout<<"Too many arguments\n";
        return 1;    
    }else{
        n = atoi(argv[2]);
        endPoint = atoi(argv[1]);
        total = 2*n;
    }
    if(n <= 1){
        cout<<"Minimum number of player allowed is 2\n";
        return 1;
    }

    //variable declaration
    int ppid = getppid(), scount=0, lcount=0,f=1,pipeNo,p=0,flag=0;
    int seq=1,pid,count=0,ppid1;
    int fp1[total][2],random = 1,input,temp,num;
    vector<pair<int,int>> snake, ladder, points;
    vector<int> playerid;
    char ch;
    string line;

    //task2 : store command line argument values to vector
    ifstream myfile (argv[3]);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            string arr[3];
            int i = 0;
            stringstream ssin(line);
            while (ssin.good() && i < 3){
                ssin >> arr[i];
                ++i;
            }
            int start = stoi(arr[1]), end = stoi(arr[2]);
            if(arr[0] == "S"){
                snake.push_back(make_pair(start,end));
                scount++;
            }else if(arr[0] == "L"){
                ladder.push_back( make_pair(start,end) );
                lcount++;
            }else{
                cout<<"Only L and S is allowed as first value...check file content\n";
                return 0;
            }
        }
        sort(snake.begin(),snake.end());
        sort(ladder.begin(),ladder.end());
        myfile.close();
    }else{
        cout<<argv[3]<<" File does not exist!!! please write correct file name!!\n";
        return 0;
    }

    //task3 : testing input file satisfies games contraints or not
    if(!file_is_well(snake,ladder,endPoint)){
        //cout<<"snake should downgrade the position and ladder should upgrade\n";
        return 0;
    }

    //for just showing the output nothing else
    cout<<"seqNo\t";
    for(int i=0; i<n; i++){
        cout<<"player"<<i+1<<"\t";
    }
    cout<<endl;

    //task4 : initialize all players position to 0
    for(int k=0; k<n; k++){
        points.push_back(make_pair(k,0));
    }

    //task5 : create 2 pipes for each player
    for(int i=0; i<total; i++){
            pipe(fp1[i]);
    }

    //task6 : create N number of players
    pid = fork();
    if(pid==0){
        exit(0);
    }
    ppid1 = getppid();
    for(int i=0; i<=n-1; i++){
        if(ppid1 == ppid){
            //fork child process
            pid = fork();
            pipeNo = count;
            ppid1 = getppid();
            count++;

            //store the process id the player
            if(pid != 0){
                playerid.push_back(pid);
            }

            //getout player i have no need of you
            if(pid==0){
                break;
            }
        }
    }

    //task7 : select random player for first chance
    srand(time(NULL)); 
    p=rand()%n;

    //task8 : Game begins....
    while(1){
        if(ppid == ppid1){          

            //close ends of the pipes 0 means read and 1 means write   
            close(fp1[2*p][0]);
            close(fp1[2*p+1][1]);

            //read value from the pipe
            read(fp1[2*p+1][0],&input,sizeof(int));
            cout<<"chance of player"<<p+1<<" value = "<<input<<endl;

            // calculating new position of player  
            temp = findNewValue(snake,ladder,input,points[p].second,atoi(argv[1]));

            //if temp returns -1 than some player won the game so kill all players and exit
            if(temp == -1){
                cout<<"Player"<<p+1<<" won the game.....congratulation!!!!"<<endl;
                for(int i=0; i<n; i++){
                    kill(playerid[i],SIGKILL);
                }
                wait(NULL);
                exit(0);
            }      

            //update position
            points[p].second = temp;

            //showing output on screen
            cout<<seq<<"\t";
            for(int i=0;i<n;i++){
                cout<<points[i].second<<"\t";
            }
            cout<<endl<<endl;

            //if player throws dice and 6 comes then player gets one more chance
            if(input!=6){
                p = (p+1)%n;
            }
            seq++;
        }else{
            //closing pipe for corrosponding child
            close(fp1[2*pipeNo][1]);
            close(fp1[2*pipeNo+1][0]);

            //generate random number by child
            srand(time(NULL)+getpid()*random++);
            num = (rand()+random++)%6+1;
            
            //writing generated value to the pipe
            write(fp1[2*pipeNo+1][1],&num,sizeof(int));
        } 
    }
return 0;
}
