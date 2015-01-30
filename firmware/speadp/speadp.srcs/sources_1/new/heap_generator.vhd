library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


entity heap_generator is
    Generic ( shift : in INTEGER := 0 );
    Port ( clk           : in  STD_LOGIC;
           ce            : in  STD_LOGIC;
           data_out_real : out STD_LOGIC_VECTOR(8 - 1 downto 0);
           data_out_comp : out STD_LOGIC_VECTOR(8 - 1 downto 0));
end heap_generator;

architecture Behavioral of heap_generator is

constant inc : integer := 4 + shift;
signal real_data, comp_data : STD_LOGIC_VECTOR (8 - 1 downto 0);

begin

    -- register all signals
    registers : process(clk)
    begin
        if (rising_edge(clk)) then
            if (ce = '1') then
                data_out_real <= real_data;
                data_out_comp <= comp_data;
            else
                data_out_real <= (OTHERS => '0');
                data_out_comp <= (OTHERS => '0');
            end if;
        end if;
    end process;

    -- data generator counter
    real_counter : entity work.counter 
    Generic Map ( count_size => 8, increment_by => inc )
    Port Map (
        clk      => clk,
        reset    => '0',
        data_out => real_data);

    comp_counter : entity work.counter 
    Generic Map ( count_size => 8, increment_by => inc )
    Port Map (
        clk      => clk,
        reset    => '0',
        data_out => comp_data);

end Behavioral;
