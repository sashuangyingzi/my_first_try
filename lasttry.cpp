#include <bits/stdc++.h>
#define True 1
#define False 0
#define Error -1
#define time_limit 180000
#define GA (ga*)malloc(sizeof(ga))
#define SE (se*)malloc(sizeof(se))
#define first_character head_of_sentence
using namespace std;
time_t op,ed;
//定义文字结构体
typedef struct sentence{
    int character;
    struct sentence *next_character;
}se;
//定义子句结构体
typedef struct gather{
    int num_of_character;
    se * head_of_sentence;
    struct gather *next_sentence;
}ga;
//定义栈结构体
struct {
    int top;//栈顶
    int f[1000];//记录是第几次
}S;
//记录链表信息
struct zhan{
    ga *one_gather;
    int list_frequency[50000][2];
    int v;
} li[1000];
ga *root,*o;//根结点
int num_of_boolean,num_of_sentences;//记录布尔变元数和子句数
int frequency[50000][2]={0};//存储次数
int strategy[10000]={0};//存储结果
int sudoku[200][4]={0};//存储数独
void display(void);//交互界面
void sat_display(void);//sat交互界面
void append_cnf(char file_name[100],char file_cnf[100]);//获得cnf后缀
void append_res(char file_name[100],char file_res[100]);//获得res后缀
int cnfparser(char file_cnf[100]);//读取cnf文件
void print_list(ga *Root);//打印list
int solver(int ch);//DPLL模块
void copylist(ga *son,ga *father);//复制链表
void copyfrequency(int son[50000][2],int father[50000][2]);//复制次数
void destroy(ga *list);//销毁链表
int search_single(ga *list);//寻找单子句
void get_S1(int x,ga *list,int frequency[50000][2]);//单子句化简
int judge_empty(ga *list);//判空集
void get_S2(int x,ga *list,int frequency[50000][2]);//分裂化简
int judge_delete(ga *list);//判空子集
int choose(ga *list,int frequency[50000][2],int ch);//策略
void print_res(int f,char file_res[100],double t);//写入res
void exam(void);//验证
void sudoku_display(void);//数独交互界面
void get_sudoku(void);//读取数独
void solve_sudoku(char file_name[100]);//解析得到数独
void translate_cnf(char file_name[100]);//将数独翻译为cnf文件
int main(){
    display();
    return 0;
}
//交互界面
void display(void){
    cout<<"请选择你要进行的操作:"<<endl;
    cout<<"1.求解sat 2.求解双数独\n";
    int choice;
    cin>>choice;
    switch(choice){
        case 1:{
            sat_display();
            break;
        }
        case 2:{
            sudoku_display();
            break;
        }
    }
}
//sat交互界面
void sat_display(void){
    char file_name[100];
    char file_cnf[100];
    char file_res[100];
    cout<<"请输入文件名(不含后缀)!\n";
    cin>>file_name;
    append_cnf(file_name,file_cnf);
    append_res(file_name,file_res);
    if(cnfparser(file_cnf)==False){
        cout<<"打开失败!\n";
        return;
    }
    cout<<"打开成功,正在解析中(限定时间为"<<time_limit<<"ms)"<<endl;
    // print_list(root);
    root->head_of_sentence=NULL;
    int f;
    op=clock();
    f=solver(1);
    ed=clock();
    double endtime=(ed-op);
    print_res(f,file_res,endtime);
    cout<<"是否验证?1:是 2:否\n";
    int cho;
    cin>>cho;
    if(cho==1) exam();
    cout<<"是否需求优化率?1:是 2:否\n";
    cin>>cho;
    // print_list(root);
    if(cho==1) {
        if(o&&o!=root) destroy(o);
        o=NULL;
        for(int i=1;i<=1000;i++){
            S.f[i]=0;
            if(li[i].one_gather&&li[i].one_gather!=root){
                destroy(li[i].one_gather);
                li[i].one_gather=NULL;
            }
        }
        op=clock();
        f=solver(2);
        ed=clock();
        double endtime2=ed-op;
        double bilv=(endtime2-endtime)/endtime*100;
        if(endtime2==0||endtime==0) {
            cout<<"精度不足,无法比较优化率"<<endl;
        }
        else if(f==-1) cout<<"优化前方案超时!\n";
        else cout<<"优化率为"<<bilv<<"%"<<endl;
    }
}
//获得cnf后缀
void append_cnf(char file_name[100],char file_cnf[100]){
    int i=0;
    while(file_name[i]){
        file_cnf[i]=file_name[i];
        i++;
    }
    file_cnf[i++]='.';
    file_cnf[i++]='c';
    file_cnf[i++]='n';
    file_cnf[i++]='f';
    file_cnf[i]='\0';
}
//获得res后缀
void append_res(char file_name[100],char file_res[100]){
    int i=0;
    while(file_name[i]){
        file_res[i]=file_name[i];
        i++;
    }
    file_res[i++]='.';
    file_res[i++]='r';
    file_res[i++]='e';
    file_res[i++]='s';
    file_res[i]='\0';
}
//读取cnf文件
int cnfparser(char file_cnf[100]){
    root=GA;
    FILE *fp;
    fp=fopen(file_cnf,"r");
    if(!fp) return False;
    char cnf[100];
    while(1){
        fscanf(fp,"%s",cnf);
        if(strcmp(cnf,"cnf")==0){
            fscanf(fp,"%d%d",&num_of_boolean,&num_of_sentences);
            break;
        }
    }
    ga *p;
    p=root;
    for(int i=0;i<num_of_sentences;i++){
        int x;
        p->next_sentence=GA;
        p->next_sentence->num_of_character=0;
        p->next_sentence->head_of_sentence=SE;
        se *q;
        q=p->next_sentence->head_of_sentence;
        while(1){
            fscanf(fp,"%d",&x);
            if(x==0){
                q->next_character=NULL;
                break;
            }
            if(x>0) frequency[x][0]++;
            else frequency[-x][1]++;
            q->next_character=SE;
            q->next_character->character=x;
            p->next_sentence->num_of_character++;
            q=q->next_character;
        }
        p=p->next_sentence;
    }
    p->next_sentence=NULL;
    fclose(fp);
    return True;
}
//打印list
void print_list(ga *Root){
    ga *p;
    p=Root->next_sentence;
    while(p){
        se *q;
        q=p->head_of_sentence->next_character;
        while(q){
            cout<<q->character<<" ";
            q=q->next_character;
        }
        cout<<endl;
        p=p->next_sentence;
    }
}
//DPLL模块
int solver(int ch){
    S.top=1;
    li[1].one_gather=GA;
    copylist(li[1].one_gather,root);
    copyfrequency(li[1].list_frequency,frequency);
    // print_list(li[S.top].one_gather);
    while(S.top>0){
        if(clock()-op>time_limit) return Error;
        if(S.f[S.top]==2){
            S.f[S.top]=0;
            destroy(li[S.top].one_gather);
            li[S.top].one_gather=NULL;
            S.top--;
        }
        else if(S.f[S.top]==1){
            int u=-li[S.top].v;
            if(u>0) strategy[u]=1;
            else strategy[-u]=-1;
            o=GA;
            copylist(o,li[S.top].one_gather);
            int fre[50000][2];
            copyfrequency(fre,li[S.top].list_frequency);
            get_S1(u,o,fre);
            if(judge_empty(o)==True) return True;
            get_S2(u,o,fre);
            if(judge_delete(o)==False){
                S.f[S.top]++;
                destroy(o);
                o=NULL;
            }
            else{
                S.f[S.top]++;
                li[++S.top].one_gather=GA;
                copylist(li[S.top].one_gather,o);
                copyfrequency(li[S.top].list_frequency,fre);
                destroy(o);
                o=NULL;
            }
        }
        else if(S.f[S.top]==0){
            int flag=1;
            while(flag){
                int single_character=search_single(li[S.top].one_gather);
                if(single_character==0) break;
                if(single_character>0) strategy[single_character]=1;
                else strategy[-single_character]=-1;
                get_S1(single_character,li[S.top].one_gather,li[S.top].list_frequency);
                if(judge_empty(li[S.top].one_gather)==True) return  True;
                get_S2(single_character,li[S.top].one_gather,li[S.top].list_frequency);
                if(judge_delete(li[S.top].one_gather)==False){
                    flag=0;
                    destroy(li[S.top].one_gather);
                    li[S.top].one_gather=NULL;
                    S.top--;
                }
            }
            if(flag){
                o=GA;
                copylist(o,li[S.top].one_gather);
                int fre[50000][2];
                copyfrequency(fre,li[S.top].list_frequency);
                int u=choose(o,fre,ch);
                if(u>0) strategy[u]=1;
                else strategy[-u]=-1;
                get_S1(u,o,fre);
                if(judge_empty(o)==True) return  True;
                get_S2(u,o,fre);
                li[S.top].v=u;
                if(judge_delete(o)==False){
                    S.f[S.top]++;
                    destroy(o);
                    o=NULL;
                }
                else{
                    S.f[S.top]++;
                    li[++S.top].one_gather=GA;
                    copylist(li[S.top].one_gather,o);
                    copyfrequency(li[S.top].list_frequency,fre);
                    destroy(o);
                    o=NULL;
                }
            }
        }
    }
    return False;
}
//复制链表
void copylist(ga *son,ga *father){
    ga *p1,*p2;
    p1=father;
    p2=son;
    son->head_of_sentence=NULL;
    while(p1->next_sentence){
        p2->next_sentence=GA;
        p2->next_sentence->head_of_sentence=SE;
        p2->next_sentence->num_of_character=p1->next_sentence->num_of_character;
        se *q1,*q2;
        q1=p1->next_sentence->head_of_sentence;
        q2=p2->next_sentence->head_of_sentence;
        while(q1->next_character){
            q2->next_character=SE;
            q2->next_character->character=q1->next_character->character;
            q1=q1->next_character;
            q2=q2->next_character;
        }
        q2->next_character=NULL;
        p1=p1->next_sentence;
        p2=p2->next_sentence;
    }
    p2->next_sentence=NULL;
}
//复制次数
void copyfrequency(int son[10000][2],int father[10000][2]){
    for(int i=1;i<=num_of_boolean;i++){
        son[i][0]=father[i][0];
        son[i][1]=father[i][1];
    }
}
//销毁链表
void destroy(ga *list){
    ga *head;
    ga *p;
    head=list;
    p=list;
    while(p->next_sentence){
        head=p;
        p=p->next_sentence;
        if(head->head_of_sentence){
            se *head2;
            se *p2;
            p2=head->head_of_sentence;
            head2=head->head_of_sentence;
            while(p2->next_character){
                head2=p2;
                p2=p2->next_character;
                free(head2);
            }
            free(p2);
        }
        free(head);
    }
    free(p);
}
//寻找单子句
int search_single(ga *list){
    ga *p;
    p=list->next_sentence;
    while(p){
        if(p->num_of_character==1) return p->head_of_sentence->next_character->character;
        p=p->next_sentence;
    }
    return False;
}
//单子句化简
void get_S1(int x,ga *list,int frequency[50000][2]){
    ga *p;
    p=list;
    while(p->next_sentence){
        int flag=0;
        se *q;
        q=p->next_sentence->head_of_sentence;
        while(q->next_character){
            if(q->next_character->character==x) {
                flag=1;
                break;
            }
            q=q->next_character;
        }
        if(flag){
            // se *qq;
            // qq=p->next_sentence->head_of_sentence;
            // while(qq->next_character){
            //     if(qq->next_character->character>0) frequency[qq->next_character->character][0]--;
            //     else frequency[-qq->next_character->character][1]--;
            //     qq=qq->next_character;
            // }
            se *head2;
            se *p2;
            p2=p->next_sentence->head_of_sentence;
            head2=p->next_sentence->head_of_sentence;
            p->next_sentence=p->next_sentence->next_sentence;
            while(p2->next_character){
                if(p2->next_character->character>0) frequency[p2->next_character->character][0]--;
                else frequency[-p2->next_character->character][1]--;
                head2=p2;
                p2=p2->next_character;
                free(head2);
            }
            free(p2);
        }
        else p=p->next_sentence;

    }
}
//判空集
int judge_empty(ga *list){
    if(list->next_sentence) return False;
    return True;
}
//分裂化简
void get_S2(int x,ga *list,int frequency[50000][2]){
    ga *p;
    p=list->next_sentence;
    while(p){
        se *q;
        q=p->head_of_sentence;
        while(q->next_character){
            if(q->next_character->character==-x){
                if(q->next_character->character>0)frequency[q->next_character->character][0]--;
                else frequency[-q->next_character->character][1]--;
                se *qq;
                qq=q->next_character;
                q->next_character=q->next_character->next_character;
                free(qq);
                p->num_of_character--;
                break;
            }
            q=q->next_character;
        }
        p=p->next_sentence;
    }
}
//判空子集
int judge_delete(ga *list){
    ga *p;
    p=list->next_sentence;
    while(p){
        if(p->num_of_character==0) return False;
        p=p->next_sentence;
    }
    return True;
}
//策略
int choose(ga *list,int frequency[50000][2],int ch){
    if(ch==2){int MI,MJ;
    MI=1;
    MJ=0;
    int i=1;
    while(i<=num_of_boolean){
        if(frequency[i][0]>frequency[MI][MJ]){
            MI=i;
            MJ=0;
        }
        if(frequency[i][1]>frequency[MI][MJ]){
            MI=i;
            MJ=1;
        }
        i++;
    }
    if(frequency[MI][MJ]==0) return False;
    return MJ==0?MI:-MI;}
    ga *p;
    p=list->next_sentence;
    int n=50000;
    int u=0;
    int t=1;
    while(p){
        if(p->num_of_character<n) n=p->num_of_character;
        p=p->next_sentence;
    }
    p=list->next_sentence;
    while(p){
        if(p->num_of_character==n){
            se *q;
            q=p->head_of_sentence->next_character;
            while(q){
                
                    if(t){
                        u=q->character;
                        t=0;
                    }
                    else{
                        if(q->character>0){
                            if(u>0){
                                if(frequency[u][0]<frequency[q->character][0]) u=q->character;
                            }
                            else if(u<0){
                                if(frequency[-u][1]<frequency[q->character][0]) u=q->character;
                            }
                        }
                        if(q->character<0){
                            if(u>0){
                                if(frequency[u][0]<frequency[-q->character][1]) u=q->character;
                            }
                            else if(u<0){
                                if(frequency[-u][1]<frequency[-q->character][1]) u=q->character;
                            }
                        }
                    }
                
                q=q->next_character;
            }
        }
        p=p->next_sentence;
    }
    return u;
}
//写入res
void print_res(int f,char file_res[100],double t){
    FILE *fp;
    fp=fopen(file_res,"w");
    if(!fp) cout<<"写入结果失败!\n";
    fprintf(fp,"%d\n",f);
    if(f==1){
        for(int i=1;i<=num_of_boolean;i++){
            fprintf(fp,"%d ",strategy[i]);
        }
        fprintf(fp,"\n%.2lfms\n",t);
    }
    else if(f==0) fprintf(fp,"%.2lfms",t);
    fclose(fp);
    cout<<"请查看文件"<<file_res<<"!"<<endl;
}

//验证
void exam(void){
    ga *p;
    p=root->next_sentence;
    int f=0;
    while(p){
        se *q;
        q=p->head_of_sentence->next_character;
        int flag=0;
        while(q){
            if(q->character>0){
                if(strategy[q->character]==1) flag=1;
            }if(q->character<0){
                if(strategy[-q->character]==-1) flag=1;
            }
            if(flag) break;
            q=q->next_character;
        }
        if(!flag){
            f=1;
            break;
        }
        p=p->next_sentence;
    }
    p=root->next_sentence;
    while(p){
        se *q;
        q=p->head_of_sentence->next_character;
        int real=0;
        while(q){
            cout<<q->character;
            if(q->character>0) {
                if(strategy[q->character]==1) cout<<"(1) ";
                else cout<<"(-1) ";
            }
            else {
                if(strategy[-q->character]==1) cout<<"(-1) ";
                else cout<<"(1) ";
            }
            if(q->character>0){
                if(strategy[q->character]==1) real=1;
            }
            else if(strategy[-q->character]==-1) real=1;
            q=q->next_character;
        }
        char ch1,ch2;
        ch1='T';
        ch2='F';
        cout<<(real==1?ch1:ch2)<<endl;
        p=p->next_sentence;
    }
    if(f)cout<<"验证未通过!"<<endl;
    else cout<<"验证通过!"<<endl;
}
//数独交互界面
void sudoku_display(void){
    cout<<"请使用1-9的数字及.输入你的数"<<endl;
    get_sudoku();
    cout<<"请为你的数独命名(英文)"<<endl;
    char file_name[100];
    char file_cnf[100];
    char file_res[100];
    cin>>file_name;
    append_cnf(file_name,file_cnf);
    append_res(file_name,file_res);
    translate_cnf(file_cnf);
    if(cnfparser(file_cnf)==False){
        cout<<"打开失败!"<<endl;
        return;
    }
    cout<<"正在解析中"<<endl;
    // print_sentences();
    op=clock();
    int flag;
    flag=solver(1);
    ed=clock();
    double endtime=ed-op;
    print_res(flag,file_res,endtime);
    solve_sudoku(file_res);
    cout<<"是否验证?1:是2:否";
    int f;
    cin>>f;
    if(f==1) exam();
}
//读取数独
void get_sudoku(void){
    int sudu[20][20]={0};
    char ch;
    for(int i=1;i<=6;i++){
        for(int j=1;j<=9;j++) {
            cin>>ch;
            if(ch>'0'&&ch<='9') sudu[i][j]=ch-48;
            else ch=0;
        }
        getchar();
    }
    for(int i=7;i<=9;i++){
        for(int j=1;j<=15;j++) {
            cin>>ch;
            if(ch>'0'&&ch<='9') sudu[i][j]=ch-48;
            else ch=0;
        }
        getchar();
    }
    for(int i=10;i<=15;i++){
        for(int j=7;j<=15;j++) {
            cin>>ch;
            if(ch>'0'&&ch<='9') sudu[i][j]=ch-48;
            else ch=0;
        }
        getchar();
    }
    int t=1;
    for(int i=1;i<=15;i++){
        for(int j=1;j<=15;j++){
           if(sudu[i][j]>0) {
            sudoku[t][1]=i;
            sudoku[t][2]=j;
            sudoku[t][3]=sudu[i][j];
            t++;
           }
        }
    }
    
    sudoku[t][1]=-1;
}
//解析得到数独
void solve_sudoku(char file_name[100]){
    int soduko_last[20][20]={0};
    FILE *fp;
    fp=fopen(file_name,"r");
    if(!fp) {
        cout<<"解析失败!"<<endl;
        return;
    }
    int f,i;
    fscanf(fp,"%d",&f);
    if(f!=1) {
        cout<<"解析失败!"<<endl;
        return;
    }
    i=1;
    int jieguo[2000];
    while(i<=729*2){
        fscanf(fp,"%d",&jieguo[i]);
        i++;
    }
    for(int h=1;h<=15;h++){
        for(int l=1;l<=15;l++){
            if((h<7&&l>9)||(h>9&&l<7)) continue;
            if(h>9||l>9){
                for(int t=1;t<=9;t++){
                    if(jieguo[(h-6-1)*81+(l-6-1)*9+t+729]==1) soduko_last[h][l]=t;
                }
            }
            else{
                for(int t=1;t<=9;t++){
                    if(jieguo[(h-1)*81+(l-1)*9+t]==1) soduko_last[h][l]=t;
                }
            }
        }
    }
    fclose(fp);
    cout<<"数独答案为:"<<endl;
    for(int i=1;i<=15;i++){
        for(int j=1;j<=15;j++){
            if(soduko_last[i][j]==0) cout<<"  ";
            else cout<<soduko_last[i][j]<<" ";
        }
        cout<<endl;
    }
}
//将数独翻译为cnf文件
void translate_cnf(char file_name[100]){
    FILE *fp;
    fp=fopen(file_name,"w");
    int num_cnf_sentences=0;//记录子句数
    int num_cnf_boolean=729*2;//记录布尔变元数
    ga *Root,*p;
    Root=(ga *)malloc(sizeof(ga));
    Root->next_sentence=NULL;
    p=Root;
    
    //sudoku1
    //单格约束
    for(int i=1;i<=9;i++){
        for(int j=1;j<=9;j++){
            p->next_sentence=(ga*)malloc(sizeof(ga));
            p->next_sentence->first_character=(se*)malloc(sizeof(se));
            se *q;
            q=p->next_sentence->first_character;
            for(int t=1;t<=9;t++){
                q->next_character=(se*)malloc(sizeof(se));
                q->next_character->character=(i-1)*81+(j-1)*9+t;//i*100+j*10+t;
                q=q->next_character;
            }
            q->next_character=NULL;
            num_cnf_sentences++;
            p=p->next_sentence;
            int n=1;
            int m;
            while(n<=9){
                m=n+1;
                while(m<=9){
                    p->next_sentence=(ga*)malloc(sizeof(ga));
                    p->next_sentence->first_character=(se*)malloc(sizeof(se));
                    q=p->next_sentence->first_character;
                    q->next_character=(se*)malloc(sizeof(se));
                    q->next_character->next_character=(se*)malloc(sizeof(se));
                    q->next_character->next_character->next_character=NULL;
                    q->next_character->character=-((i-1)*81+(j-1)*9+n);//-(i*100+j*10+n);
                    q->next_character->next_character->character=-((i-1)*81+(j-1)*9+m);//-(i*100+j*10+m);
                    p=p->next_sentence;
                    num_cnf_sentences++;
                    m++;
                }
                n++;
            }

        }
    }
    //行约束

    for(int t=1;t<=9;t++){
        for(int j=1;j<=9;j++){
            p->next_sentence=(ga *)malloc(sizeof(ga));
            p->next_sentence->first_character=(se *)malloc(sizeof(se));
            se *q=p->next_sentence->first_character;
            for(int i=1;i<=9;i++){
                q->next_character=(se *)malloc(sizeof(se));
                q->next_character->character=(t-1)*81+(i-1)*9+j;//t*100+i*10+j;
                q=q->next_character;
            }
            q->next_character=NULL;
            p=p->next_sentence;
            num_cnf_sentences++;
        }
    }
    for(int s=1;s<=9;s++){
        int u=1;
        while(u<=9){
            int v=1;
            while(v<=9){
                int c=v+1;
                while(c<=9){
                    p->next_sentence=(ga *)malloc(sizeof(ga));
                    p->next_sentence->first_character=(se *)malloc(sizeof(se));
                    se *q;
                    q=p->next_sentence->first_character;
                    q->next_character=(se *)malloc(sizeof(se));
                    q->next_character->character=-(s+(u-1)*81+(v-1)*9);//-(s+u*100+v*10);
                    q->next_character->next_character=(se *)malloc(sizeof(se));
                    q->next_character->next_character->character=-(s+(u-1)*81+(c-1)*9);//-(s+u*100+c*10);
                    c++;
                    q->next_character->next_character->next_character=NULL;
                    p=p->next_sentence;
                    num_cnf_sentences++;

                }
                v++;
            }
            u++;
        }
    }
    //列约束
    for(int i=1;i<=9;i++){
        for(int j=1;j<=9;j++){
            p->next_sentence=(ga *)malloc(sizeof(ga));
            p->next_sentence->first_character=(se *)malloc(sizeof(se));
            se *q=p->next_sentence->first_character;
            for(int t=1;t<=9;t++){
                q->next_character=(se *)malloc(sizeof(se));
                q->next_character->character=(t-1)*81+(i-1)*9+j;//(t*100+i*10+j);
                q=q->next_character;
            }
            q->next_character=NULL;
            p=p->next_sentence;
            num_cnf_sentences++;
        }
    }
    for(int s=1;s<=9;s++){
        int u=1;
        while(u<=9){
            int v=1;
            while(v<=9){
                int c=v+1;
                while(c<=9){
                    p->next_sentence=(ga *)malloc(sizeof(ga));
                    p->next_sentence->first_character=(se *)malloc(sizeof(se));
                    se *q;
                    q=p->next_sentence->first_character;
                    q->next_character=(se *)malloc(sizeof(se));
                    q->next_character->character=-(s+(v-1)*81+(u-1)*9);//-(s+v*100+u*10);
                    q->next_character->next_character=(se *)malloc(sizeof(se));
                    q->next_character->next_character->character=-(s+(c-1)*81+(u-1)*9);//-(s+c*100+u*10);
                    c++;
                    q->next_character->next_character->next_character=NULL;
                    p=p->next_sentence;
                    num_cnf_sentences++;

                }
                v++;
            }
            u++;
        }
    }
    
    //盒子约束
    for(int num=0;num<9;num++){
        for(int n=1;n<=9;n++){
            p->next_sentence=(ga*)malloc(sizeof(ga));
            p->next_sentence->first_character=(se*)malloc(sizeof(se));
            se *q;
            q=p->next_sentence->first_character;
            int h=num/3*3+1;
            int l=num%3*3+1;
            int u,v;
            u=h;
            while(u<h+3){
                v=l;
                while(v<l+3){
                    q->next_character=(se*)malloc(sizeof(se));
                    q->next_character->character=81*(u-1)+9*(v-1)+n;//100*u+10*v+n;
                    q=q->next_character;
                    v++;
                }
                u++;
            }
            q->next_character=NULL;
            p=p->next_sentence;
            num_cnf_sentences++;
        }
    }
    for(int num=0;num<9;num++){
        for(int n=1;n<=9;n++){
            int h=num/3*3+1;
            int l=num%3*3+1;
            int t,k;
            t=0;
            while(t<9){
                k=t+1;
                while(k<9){
                    p->next_sentence=(ga *)malloc(sizeof(ga));
                    p->next_sentence->first_character=(se*)malloc(sizeof(se));
                    p->next_sentence->first_character->next_character=(se*)malloc(sizeof(se));
                    p->next_sentence->first_character->next_character->character=-((h+t/3-1)*81+(l+t%3-1)*9+n);//-((h+t/3)*100+(l+t%3)*10+n);
                    p->next_sentence->first_character->next_character->next_character=(se*)malloc(sizeof(se));
                    p->next_sentence->first_character->next_character->next_character->character=-((h+k/3-1)*81+(l+k%3-1)*9+n);//-((h+k/3)*100+(l+k%3)*10+n);
                    p->next_sentence->first_character->next_character->next_character->next_character=NULL;
                    p=p->next_sentence;
                    num_cnf_sentences++;
                    k++;
                }
                t++;
            }
        }
    }

    //sudoku2
    //单格约束
    for(int i=1;i<=9;i++){
        for(int j=1;j<=9;j++){
            p->next_sentence=(ga*)malloc(sizeof(ga));
            p->next_sentence->first_character=(se*)malloc(sizeof(se));
            se *q;
            q=p->next_sentence->first_character;
            for(int t=1;t<=9;t++){
                q->next_character=(se*)malloc(sizeof(se));
                q->next_character->character=(i-1)*81+(j-1)*9+t+729;//1000+i*100+j*10+t;
                q=q->next_character;
            }
            q->next_character=NULL;
            num_cnf_sentences++;
            p=p->next_sentence;
            int n=1;
            int m;
            while(n<=9){
                m=n+1;
                while(m<=9){
                    p->next_sentence=(ga*)malloc(sizeof(ga));
                    p->next_sentence->first_character=(se*)malloc(sizeof(se));
                    q=p->next_sentence->first_character;
                    q->next_character=(se*)malloc(sizeof(se));
                    q->next_character->next_character=(se*)malloc(sizeof(se));
                    q->next_character->next_character->next_character=NULL;
                    q->next_character->character=-((i-1)*81+(j-1)*9+n+729);//-(1000+i*100+j*10+n);
                    q->next_character->next_character->character=-((i-1)*81+(j-1)*9+m+729);//-(1000+i*100+j*10+m);
                    p=p->next_sentence;
                    num_cnf_sentences++;
                    m++;
                }
                n++;
            }

        }
    }
    //行约束
    for(int t=1;t<=9;t++){
        for(int j=1;j<=9;j++){
            p->next_sentence=(ga *)malloc(sizeof(ga));
            p->next_sentence->first_character=(se *)malloc(sizeof(se));
            se *q=p->next_sentence->first_character;
            for(int i=1;i<=9;i++){
                q->next_character=(se *)malloc(sizeof(se));
                q->next_character->character=(t-1)*81+(i-1)*9+j+729;//1000+t*100+i*10+j;
                q=q->next_character;
            }
            q->next_character=NULL;
            p=p->next_sentence;
            num_cnf_sentences++;
        }
    }
    for(int s=1;s<=9;s++){
        int u=1;
        while(u<=9){
            int v=1;
            while(v<=9){
                int c=v+1;
                while(c<=9){
                    p->next_sentence=(ga *)malloc(sizeof(ga));
                    p->next_sentence->first_character=(se *)malloc(sizeof(se));
                    se *q;
                    q=p->next_sentence->first_character;
                    q->next_character=(se *)malloc(sizeof(se));
                    q->next_character->character=-(729+s+(u-1)*81+(v-1)*9);//-(1000+s+u*100+v*10);
                    q->next_character->next_character=(se *)malloc(sizeof(se));
                    q->next_character->next_character->character=-(729+s+(u-1)*81+(c-1)*9);//-(1000+s+u*100+c*10);
                    c++;
                    q->next_character->next_character->next_character=NULL;
                    p=p->next_sentence;
                    num_cnf_sentences++;

                }
                v++;
            }
            u++;
        }
    }
    //列约束
    for(int i=1;i<=9;i++){
        for(int j=1;j<=9;j++){
            p->next_sentence=(ga *)malloc(sizeof(ga));
            p->next_sentence->first_character=(se *)malloc(sizeof(se));
            se *q=p->next_sentence->first_character;
            for(int t=1;t<=9;t++){
                q->next_character=(se *)malloc(sizeof(se));
                q->next_character->character=(t-1)*81+(i-1)*9+j+729;//(1000+t*100+i*10+j);
                q=q->next_character;
            }
            q->next_character=NULL;
            p=p->next_sentence;
            num_cnf_sentences++;
        }
    }
    for(int s=1;s<=9;s++){
        int u=1;
        while(u<=9){
            int v=1;
            while(v<=9){
                int c=v+1;
                while(c<=9){
                    p->next_sentence=(ga *)malloc(sizeof(ga));
                    p->next_sentence->head_of_sentence=(se *)malloc(sizeof(se));
                    se *q;
                    q=p->next_sentence->head_of_sentence;
                    q->next_character=(se *)malloc(sizeof(se));
                    q->next_character->character=-(729+s+(v-1)*81+(u-1)*9);//-(1000+s+v*100+u*10);
                    q->next_character->next_character=(se *)malloc(sizeof(se));
                    q->next_character->next_character->character=-(729+s+(c-1)*81+(u-1)*9);//-(1000+s+c*100+u*10);
                    c++;
                    q->next_character->next_character->next_character=NULL;
                    p=p->next_sentence;
                    num_cnf_sentences++;

                }
                v++;
            }
            u++;
        }
    }
     //盒子约束
    for(int num=0;num<9;num++){
        for(int n=1;n<=9;n++){
            p->next_sentence=(ga*)malloc(sizeof(ga));
            p->next_sentence->head_of_sentence=(se*)malloc(sizeof(se));
            se *q;
            q=p->next_sentence->head_of_sentence;
            int h=num/3*3+1;
            int l=num%3*3+1;
            int u,v;
            u=h;
            while(u<h+3){
                v=l;
                while(v<l+3){
                    q->next_character=(se*)malloc(sizeof(se));
                    q->next_character->character=81*(u-1)+9*(v-1)+n+729;//100*u+10*v+n;
                    q=q->next_character;
                    v++;
                }
                u++;
            }
            q->next_character=NULL;
            p=p->next_sentence;
            num_cnf_sentences++;
        }
    }
    for(int num=0;num<9;num++){
        for(int n=1;n<=9;n++){
            int h=num/3*3+1;
            int l=num%3*3+1;
            int t,k;
            t=0;
            while(t<9){
                k=t+1;
                while(k<9){
                    p->next_sentence=(ga *)malloc(sizeof(ga));
                    p->next_sentence->head_of_sentence=(se*)malloc(sizeof(se));
                    p->next_sentence->head_of_sentence->next_character=(se*)malloc(sizeof(se));
                    p->next_sentence->head_of_sentence->next_character->character=-((h+t/3-1)*81+(l+t%3-1)*9+n+729);//-((h+t/3)*100+(l+t%3)*10+n);
                    p->next_sentence->head_of_sentence->next_character->next_character=(se*)malloc(sizeof(se));
                    p->next_sentence->head_of_sentence->next_character->next_character->character=-((h+k/3-1)*81+(l+k%3-1)*9+n+729);//-((h+k/3)*100+(l+k%3)*10+n);
                    p->next_sentence->head_of_sentence->next_character->next_character->next_character=NULL;
                    p=p->next_sentence;
                    num_cnf_sentences++;
                    k++;
                }
                t++;
            }
        }
    }


    //已知条件
    int n=1;
    while(sudoku[n][1]!=-1){
        int x,y,z;
        x=sudoku[n][1];
        y=sudoku[n][2];
        z=sudoku[n][3];
        if(x>9||y>9){
            p->next_sentence=(ga*)malloc(sizeof(ga));
            p->next_sentence->head_of_sentence=(se*)malloc(sizeof(se));
            p->next_sentence->head_of_sentence->next_character=(se*)malloc(sizeof(se));
            p->next_sentence->head_of_sentence->next_character->next_character=NULL;
            p->next_sentence->head_of_sentence->next_character->character=729+(x-6-1)*81+(y-6-1)*9+z;
            num_cnf_sentences++;
            p=p->next_sentence;
        }
        else{
            p->next_sentence=(ga*)malloc(sizeof(ga));
            p->next_sentence->head_of_sentence=(se*)malloc(sizeof(se));
            p->next_sentence->head_of_sentence->next_character=(se*)malloc(sizeof(se));
            p->next_sentence->head_of_sentence->next_character->next_character=NULL;
            p->next_sentence->head_of_sentence->next_character->character=(x-1)*81+(y-1)*9+z;
            num_cnf_sentences++;
            p=p->next_sentence;
        }
        n++;
    }
    
    //等价约束
    int h=7;
    int l=7;
    for(int num=1;num<=9;num++){
        int x=0;
        while(x<9){
            int h1=x/3+h;
            int h2=x/3+h-6;
            int l1=x%3+l;
            int l2=x%3+l-6;
            p->next_sentence=(ga*)malloc(sizeof(ga));
            p->next_sentence->head_of_sentence=(se*)malloc(sizeof(se));
            p->next_sentence->head_of_sentence->next_character=(se*)malloc(sizeof(se));
            p->next_sentence->head_of_sentence->next_character->character=(h1-1)*81+(l1-1)*9+num;//h1*100+l1*10+num;
            p->next_sentence->head_of_sentence->next_character->next_character=(se*)malloc(sizeof(se));
            p->next_sentence->head_of_sentence->next_character->next_character->character=-((h2-1)*81+(l2-1)*9+num+729);//-(h2*100+l2*10+num+1000);
            p->next_sentence->head_of_sentence->next_character->next_character->next_character=NULL;
            p=p->next_sentence;
            p->next_sentence=(ga*)malloc(sizeof(ga));
            p->next_sentence->head_of_sentence=(se*)malloc(sizeof(se));
            p->next_sentence->head_of_sentence->next_character=(se*)malloc(sizeof(se));
            p->next_sentence->head_of_sentence->next_character->character=-((h1-1)*81+(l1-1)*9+num);//-(h1*100+l1*10+num);
            p->next_sentence->head_of_sentence->next_character->next_character=(se*)malloc(sizeof(se));
            p->next_sentence->head_of_sentence->next_character->next_character->character=(h2-1)*81+(l2-1)*9+num+729;//(h2*100+l2*10+num+1000);
            p->next_sentence->head_of_sentence->next_character->next_character->next_character=NULL;
            p=p->next_sentence;
            num_cnf_sentences+=2;
            x++;
        }
    }

    p->next_sentence=NULL;
    fprintf(fp,"cnf %d %d\n",num_cnf_boolean,num_cnf_sentences);
    p=Root->next_sentence;
    while(p){
        se *q;
        q=p->head_of_sentence->next_character;
        while(q){
            fprintf(fp,"%d ",q->character);
            q=q->next_character;
        }
        fprintf(fp,"0");
        if(p->next_sentence) fprintf(fp,"\n");
        p=p->next_sentence;
    }
    fclose(fp);
}
