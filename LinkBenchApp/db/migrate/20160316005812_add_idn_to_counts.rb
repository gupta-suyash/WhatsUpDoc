class AddIdnToCounts < ActiveRecord::Migration
  def change
    add_column :counts, :id1, :integer
  end
end
