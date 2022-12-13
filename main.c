#include <stdio.h>
#include <string.h>

#define MAXROWS 41
#define MAXCOLS 180

int loadInputFile();
int nextLine(char *dst);

char line[200];

typedef struct
{
    int i;
    int j;
} pos;

struct queue
{
    pos items[10000];
    int front;
    int rear;
};

#define init(s) ((s).front = 0; (s).rear = 0)
#define isempty(s) ((s).front == (s).rear)
#define any(s) ((s).front != (s).rear)
#define push(s, x) ((s).items[((s).rear++)] = (x))
#define pop(s) ((s).items[(s).front++])
#define siz(s) ((s).rear - (s).front)

void loadMountain(char mount[][MAXCOLS], int *pnrows, int *pncols)
{
    int ncols = nextLine(line);
    int nrows = 0;
    do
    {
        strncpy(mount[nrows], line, ncols);
        nrows++;
    } while (nextLine(line) > 0);
    *pnrows = nrows;
    *pncols = ncols;
}

void locateSandE(pos *pS, pos *pE, char mount[][MAXCOLS], int nrows, int ncols)
{
    pS->i = -1;
    pE->i = -1;
    for (int i = 0; (pS->i == -1 || pE->i == -1) && i < nrows; i++)
    {
        for (int j = 0; (pS->i == -1 || pE->i == -1) && j < ncols; j++)
        {
            if (mount[i][j] == 'S')
            {
                mount[i][j] = 'a';
                pS->i = i;
                pS->j = j;
            }
            else if (mount[i][j] == 'E')
            {
                mount[i][j] = 'z'; // One exception less to consider!
                pE->i = i;
                pE->j = j;
            }
        }
    }
}

char mountain[MAXROWS][MAXCOLS];
int stepsNeeded[MAXROWS][MAXCOLS];

#define valid(u, v) ((v).i >= 0 && (v).i < nrows && (v).j >= 0 && (v).j < ncols && stepsNeeded[(v).i][(v).j] < 0 && mountain[(u).i][(u).j] - mountain[(v).i][(v).j] <= 1)

struct queue queue;
int main()
{
    if (loadInputFile() < 0)
    {
        fprintf(stderr, "Can't open: input.txt\n");
        return 1;
    }

    int nrows, ncols;
    loadMountain(mountain, &nrows, &ncols);
    // fill with neg numbers, -1 in systems with two's complement negative int
    memset(stepsNeeded, 0xff, sizeof(stepsNeeded));

    pos p, target;
    locateSandE(&p, &target, mountain, nrows, ncols);

    // BFS: first time we reach a pos, it will be with minimum steps
    stepsNeeded[target.i][target.j] = 0;
    pos r;
    // Go backwards! minimum change in 'valid()' macro
    push(queue, target);
    int minSteps = 1000000000;
    while (any(queue))
    {
        p = pop(queue);
        if(mountain[p.i][p.j] == 'a' && stepsNeeded[p.i][p.j] < minSteps)
        {
            minSteps = stepsNeeded[p.i][p.j];
        }
        // up?
        r.i = p.i - 1;
        r.j = p.j;
        if (valid(p, r))
        {
            stepsNeeded[r.i][r.j] = 1 + stepsNeeded[p.i][p.j];
            push(queue, r);
        }
        // down?
        r.i = p.i + 1;
        if (valid(p, r))
        {
            stepsNeeded[r.i][r.j] = 1 + stepsNeeded[p.i][p.j];
            push(queue, r);
        }
        // right ?
        r.i = p.i;
        r.j = p.j + 1;
        if (valid(p, r))
        {
            stepsNeeded[r.i][r.j] = 1 + stepsNeeded[p.i][p.j];
            push(queue, r);
        }

        // left ?
        r.j = p.j - 1;
        if (valid(p, r))
        {
            stepsNeeded[r.i][r.j] = 1 + stepsNeeded[p.i][p.j];
            push(queue, r);
        }
    }

    printf("%d\n", minSteps);
}

#define BUFSIZE 1 << 13

char fileBuf[BUFSIZE];
char *pbuf = fileBuf;
char *pbufend = fileBuf;

int loadInputFile()
{
    FILE *pf = fopen("input.txt", "r");
    if (!pf)
        return -1;
    pbuf = fileBuf;
    pbufend = fileBuf + fread(fileBuf, 1, BUFSIZE, pf);
    fclose(pf);

    return 0;
}

int nextLine(char *dst)
{
    char c;
    char *orig = dst;

    // read line:
    while (pbuf < pbufend && (c = *(pbuf++)) != '\n')
    {
        *(dst++) = c;
    }
    *dst = '\0';
    // return line length
    return dst - orig;
}