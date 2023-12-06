#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int err(char *s)
{
    while(*s)
        write(2, s++, 1);
    return 1;
}

int cd(char **av, int i)
{
    if (i != 2)
        return(err("error: cd: bad arguments\n"));
    else if (chdir(av[1]) == -1)
        return (err("error: cd: cannot change directory to "), err(av[1])), err("\n");
    return 0;
}

int	execute(char **av, char **envp, int i)
{
	int	fd[2];
	int	status;

	int has_pipe = av[i] && !strcmp(av[i], "|");
	if (has_pipe && pipe(fd) == -1)
		return (err("error: fatal\n"));
	int pid = fork();
	if (!pid)
	{
		av[i] = 0; // Set the element after the pipe or semicolon to null
		if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
			return (err("error: fatal\n"));
		execve(*av, av, envp);
		return (err("error: cannot execute "), err(*av), err("\n"));
	}
	waitpid(pid, &status, 0);
	if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		return (err("error: fatal\n"));
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	int i = 0;

	if (ac == 1)
		return (0);
	while (av[i] && av[++i])
	{
		av = &av[i];
		i = 0;
		while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
			i++;
		if (!strcmp(av[0], "cd"))
			cd(av, i);
		else if (i)
			execute(av, envp, i);
	}
	return (0);
}