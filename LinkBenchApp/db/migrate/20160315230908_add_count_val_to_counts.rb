class AddCountValToCounts < ActiveRecord::Migration
  def change
    add_column :counts, :countval, :integer
  end
end
