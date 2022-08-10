#include "Response.hpp"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t tmpsize;
	size_t i;

	if (!dst || !src)
		return (0);
	tmpsize = size;
	i = 0;
	while (src[i] != '\0' && tmpsize > 0)
	{
		dst[i] = src[i];
		tmpsize--;
		i++;
	}
	if (i < size)
		dst[i] = '\0';
	else if (size > 0)
		dst[i - 1] = '\0';
	return (strlen(src));
}

static const char	*skipDelimeters(const char *s, char c)
{
	while (*s == c)
		s++;
	return (s);
}

static int			countWords(const char *s, char c)
{
	int				result;

	result = 0;
	while (*s != '\0')
	{
		s = skipDelimeters(s, c);
		if (*s != '\0')
		{
			result++;
			while (*s != '\0' && *s != c)
				s++;
		}
	}
	return (result);
}

static char			**freeSpace(char **to_return)
{
	int				i;

	i = 0;
	while (to_return[i])
		free(to_return[i++]);
	free(to_return);
	return (NULL);
}

static int			get_len(char const *s, char c)
{
	int				i;

	i = 0;
	while (*s != '\0' && *s != c)
	{
		i++;
		s++;
	}
	return (i);
}

char				**ftSplit(char const *s, char c)
{
	char	**to_return;
	int		words_amount;
	int		cur_length;
	int		index;

	index = 0;
	words_amount = countWords(s, c);
	if (!(to_return = (char**)malloc(sizeof(char*) * (words_amount + 1))))
		return (NULL);
	while (index < words_amount)
	{
		s = skipDelimeters(s, c);
		cur_length = get_len(s, c);
		if (!(to_return[index] = (char*)malloc(sizeof(char) *
						(cur_length + 1))))
			return (freeSpace(to_return));
		ft_strlcpy(to_return[index], s, cur_length + 1);
		s += cur_length;
		index++;
	}
	to_return[index] = NULL;
	return (to_return);
}

void	freeArray(char **array)
{
	int		i;

	i = 0;
	while (array && array[i])
	{
		free(array[i]);
		array[i] = NULL;
		i++;
	}
	free(array);
	array = NULL;
}
