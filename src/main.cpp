#include <iostream>
#include <vector>
#include <string>
#include <pthread.h>

typedef struct
{
    double * vec;
    size_t sub_from;
    size_t what_sub;
    size_t length;
} vec_with_ind;

void * to_diagonal(void * data)
{
    double * equation = ((vec_with_ind *)data)->vec;
    size_t sub_from = ((vec_with_ind *)data)->sub_from;
    size_t what_sub = ((vec_with_ind *)data)->what_sub;
    size_t length = ((vec_with_ind *)data)->length;
    
    double mult = equation[sub_from + (what_sub / length)] / equation[what_sub + (what_sub / length)];
    //std::cout << sub_from << " " << what_sub << std::endl;

    for (size_t i = sub_from; i < sub_from + length; ++i)
    {
        //std::cout << sub_from << ") " << equation[i] << std::endl;
        equation[i] = equation[i] - equation[what_sub + i - sub_from] * mult;
        //std::cout << sub_from << ") " << equation[i] << std::endl;
    }

    pthread_exit(0);
}

void solve_system(double * vec, size_t n, size_t m, int threads)
{
    pthread_t tid[threads];
    for (size_t i = 0; i < n*(m+1); i = i + m + 1)
    {
        for (size_t j = 0; j < n*(m+1); j = j + m + 1)
        {
            int created = 0;
            while ((created != threads) and (j < n*(m+1)))
            {
                
                if (i == j)
                {
                    j = j + m + 1;
                    continue;
                }
                
                vec_with_ind * data = (vec_with_ind *)malloc(sizeof(vec_with_ind));
                data->vec = vec;
                data->sub_from = j;
                data->what_sub = i;
                data->length = m + 1;
                pthread_create(&tid[created], NULL, to_diagonal, data);
                
                created++;
                j = j + m + 1;
            }
            j = j - m - 1;

            for (size_t k = 0; k < threads; ++k) pthread_join(tid[k], NULL);
        }
    }
    
}

void print_system (double * vec, size_t n, size_t m)
{
    for (size_t i = 0; i < n*(m+1); ++i)
    {
        if ((i+1) % (m+1) == 0 and i != 0)
            std::cout << "| ";
        std::cout << vec[i] << " "; 
        if ((i+1) % (m+1) == 0 and i != 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main (int argc, char * argv[])
{
    if (argc != 2)
    {
        std::cerr << "Incorrect arguments." << std::endl;
        return 1;
    }

    int threads = atoi(argv[1]);

    std::cout << "Enter the number of equations of the system:" << std::endl;
    int n_of_eq;
    std::cin >> n_of_eq;

    std::cout << "Enter the number of variables in the equations:" << std::endl;
    int n_of_var;
    std::cin >> n_of_var;

    if (n_of_eq <= 0 or n_of_var <= 0)
    {
        std::cerr << "Incorrect parameters." << std::endl;
        return 1;
    }

    if (n_of_eq != n_of_var)
    {
        std::cerr << "There may be no or an infinite number of solutions." << std::endl;
        return 1;
    }

    std::cout << "Enter the equations: " << std::endl;

    double s_of_eq[n_of_eq * (n_of_var + 1)];
    size_t zero_cnt = 0;
    for (size_t i = 0; i < n_of_eq * (n_of_var + 1); ++i)
    {
        std::cin >> s_of_eq[i];

        if (s_of_eq[i] == 0) zero_cnt++;

        if (((i + 1) % (n_of_var + 1) == 0) and (i != 0)) 
        {
            if ((zero_cnt == n_of_var) and (s_of_eq[i] != 0))
            {
                std::cerr << "There are no solutions." << std::endl;
                return 1;
            }
            zero_cnt = 0;
        }
    }
    std::cout << std::endl;

    std::cout << "Your system of equations: " << std::endl;
    print_system(s_of_eq, n_of_eq, n_of_var);

    auto start = std::chrono::high_resolution_clock::now();

    solve_system(s_of_eq, n_of_eq, n_of_var, threads);

    std::cout << "The system of equations in diagonal form: " << std::endl;
    print_system(s_of_eq, n_of_eq, n_of_var);

    
    size_t ind = 1;
    for (size_t i = 0; i < n_of_eq * (n_of_var+1); i = i + n_of_var + 2)
    {
        std::cout << "x" << ind << ") " << s_of_eq[i + (n_of_var - (i / (n_of_var+1)))] / s_of_eq[i] << std::endl;
        ind++;
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "The system of equations is solved in " << time.count() << " microseconds." << std::endl;
}
